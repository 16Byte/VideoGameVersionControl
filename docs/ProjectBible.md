# VideoGameVersionControl - Implementation Bible

> **Purpose:** This document is the single source of truth for building VGVC. Follow these architectural decisions, coding standards, and implementation phases religiously. When AI suggests something different, this document wins.

---

## Table of Contents
1. [Architectural Principles](#architectural-principles)
2. [Project Structure](#project-structure)
3. [Technology Stack & Dependencies](#technology-stack--dependencies)
4. [Core Components](#core-components)
5. [Coding Standards](#coding-standards)
6. [Implementation Phases](#implementation-phases)
7. [Testing Strategy](#testing-strategy)
8. [Common Pitfalls](#common-pitfalls)

---

## Architectural Principles

### The Non-Negotiables

**1. Separation of Concerns**
- **UI Layer:** Qt Widgets, pure presentation logic
- **Service Layer:** Business logic, no Qt widgets
- **Data Layer:** Git operations, file I/O, no business rules

**2. Qt Best Practices**
- Use Model/View architecture for all lists
- All long-running operations use `QtConcurrent` or `QThread`
- Signals/slots for async communication only
- Never block the UI thread
- RAII for all resource management (no manual `delete`)

**3. Git Abstraction**
- All Git operations go through `GitService` class
- Use `QProcess` to spawn git CLI commands
- Parse output into structured C++ objects
- Never expose Git concepts (commits, branches, HEAD) to UI

**4. Error Handling**
- Every operation returns `Result<T, Error>` pattern
- User-facing error messages must be actionable
- Log technical details, show friendly messages to users
- No uncaught exceptions in production code

**5. Async Everything**
- File operations: async
- Git operations: async
- Network operations: async
- Use `QFuture` for results, emit signals when complete

---

## Project Structure

```
VideoGameVersionControl/
├── CMakeLists.txt                 # Root build configuration
├── README.md
├── LICENSE
├── docs/
│   ├── DesignBrief.md            # High-level vision
│   └── ImplementationBible.md    # This file
├── src/
│   ├── CMakeLists.txt
│   ├── main.cpp                  # Entry point
│   ├── core/                     # Business logic (no Qt Widgets)
│   │   ├── GitService.h/cpp      # Git CLI wrapper
│   │   ├── SnapshotManager.h/cpp # Snapshot operations
│   │   ├── PresetManager.h/cpp   # Game preset handling
│   │   ├── ProjectConfig.h/cpp   # Per-project settings
│   │   └── types/                # Data structures
│   │       ├── Snapshot.h
│   │       ├── GamePreset.h
│   │       └── Result.h          # Result<T, E> implementation
│   ├── ui/                       # Qt Widgets UI
│   │   ├── MainWindow.h/cpp      # Main application window
│   │   ├── SnapshotListWidget.h/cpp
│   │   ├── CreateSnapshotDialog.h/cpp
│   │   ├── RestoreDialog.h/cpp
│   │   ├── SettingsDialog.h/cpp
│   │   └── models/               # Qt models for views
│   │       └── SnapshotListModel.h/cpp
│   └── utils/                    # Utilities
│       ├── FileUtils.h/cpp       # File operations
│       ├── PathDetector.h/cpp    # Game path detection
│       └── Logger.h/cpp          # Logging system
├── resources/
│   ├── presets/                  # Built-in game presets
│   │   ├── kerbal_space_program.json
│   │   ├── minecraft_java.json
│   │   └── skyrim_se.json
│   ├── icons/                    # Application icons
│   └── ui/                       # Qt Designer .ui files (optional)
├── tests/
│   ├── CMakeLists.txt
│   ├── test_gitservice.cpp
│   ├── test_snapshotmanager.cpp
│   └── test_presetmanager.cpp
└── scripts/
    ├── setup_dev_env.sh          # Development environment setup
    └── package.sh                # Packaging script per platform
```

---

## Technology Stack & Dependencies

### Core Technologies
- **C++17** (minimum standard)
- **Qt 6.5+** (LTS preferred)
- **CMake 3.20+**
- **Git 2.30+** (runtime dependency)

### Qt Modules Required
```cmake
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Concurrent
    Network      # For cloud sync (Phase 2)
)
```

### Third-Party Libraries
- **None for MVP** (keep it simple)
- Post-MVP: `nlohmann/json` or Qt's JSON if insufficient
- Testing: Qt Test framework (built-in)

### Platform-Specific Requirements

**Windows:**
- MSVC 2019+ or MinGW-w64
- Bundle portable Git (2.30+) in installer
- Use `git.exe` from bundled path

**macOS:**
- Xcode Command Line Tools (includes git)
- Target macOS 12+
- Universal binary (x86_64 + arm64)

**Linux:**
- GCC 9+ or Clang 10+
- Git installed via system package manager
- Flatpak runtime: `org.kde.Platform` version 6.6

---

## Core Components

### 1. GitService (core/GitService.h)

**Responsibility:** Execute Git commands and parse output

**Interface:**
```cpp
class GitService : public QObject {
    Q_OBJECT
public:
    explicit GitService(const QString& repoPath, QObject* parent = nullptr);
    
    // Async operations
    QFuture<Result<void, QString>> init();
    QFuture<Result<void, QString>> commit(const QString& message);
    QFuture<Result<QList<Snapshot>, QString>> getHistory(int limit = 50);
    QFuture<Result<void, QString>> checkout(const QString& commitHash);
    QFuture<Result<qint64, QString>> getRepoSize();
    
signals:
    void operationProgress(int percentage, const QString& status);
    
private:
    QString m_repoPath;
    QString m_gitExecutable;  // Path to git binary
    
    Result<QString, QString> executeGitCommand(const QStringList& args);
    Result<Snapshot, QString> parseCommitLog(const QString& logLine);
};
```

**Implementation Rules:**
- Use `QProcess::start()` with `QProcess::waitForFinished()`
- Set working directory to `m_repoPath` before executing
- Parse stdout line-by-line
- Capture stderr for error messages
- Timeout all operations (30s for most, 5min for clone/pull)
- Use `--porcelain` flags where available for stable output

**Git Commands to Support (MVP):**
```bash
# Init
git init
git config user.name "VGVC User"
git config user.email "vgvc@local"

# Commit
git add .
git commit -m "message"

# History
git log --pretty=format:"%H|%an|%at|%s" -n 50

# Checkout
git checkout <hash>

# Size
git count-objects -vH
```

---

### 2. SnapshotManager (core/SnapshotManager.h)

**Responsibility:** High-level snapshot operations

**Interface:**
```cpp
class SnapshotManager : public QObject {
    Q_OBJECT
public:
    explicit SnapshotManager(GitService* gitService, QObject* parent = nullptr);
    
    QFuture<Result<void, QString>> createSnapshot(const QString& description);
    QFuture<Result<QList<Snapshot>, QString>> listSnapshots();
    QFuture<Result<void, QString>> restoreSnapshot(const QString& snapshotId);
    QFuture<Result<void, QString>> deleteSnapshot(const QString& snapshotId);
    
signals:
    void snapshotCreated(const Snapshot& snapshot);
    void snapshotRestored(const QString& snapshotId);
    
private:
    GitService* m_gitService;
    
    QString generateDescription(const QString& userDescription);
};
```

**Implementation Rules:**
- Generate timestamps in ISO8601 format
- Auto-generate description if user leaves blank: "Snapshot - YYYY-MM-DD HH:MM"
- Before restore, create automatic safety snapshot
- Emit progress signals during long operations

---

### 3. PresetManager (core/PresetManager.h)

**Responsibility:** Load and manage game presets

**Interface:**
```cpp
struct GamePreset {
    QString gameId;
    QString displayName;
    QMap<QString, QStringList> detectionPaths;  // platform -> paths
    QStringList trackedPaths;
    QStringList ignorePatterns;
    qint64 largeFileWarningMB;
};

class PresetManager : public QObject {
    Q_OBJECT
public:
    explicit PresetManager(QObject* parent = nullptr);
    
    Result<QList<GamePreset>, QString> loadBuiltInPresets();
    Result<GamePreset, QString> loadPreset(const QString& presetId);
    Result<QString, QString> detectGame(const QString& path);
    Result<void, QString> applyPreset(const QString& repoPath, const GamePreset& preset);
    
private:
    QMap<QString, GamePreset> m_presets;
    
    Result<GamePreset, QString> parsePresetFile(const QString& filePath);
    void createGitignore(const QString& repoPath, const QStringList& patterns);
};
```

**Preset File Format:**
```json
{
  "game_id": "skyrim_se",
  "display_name": "The Elder Scrolls V: Skyrim Special Edition",
  "detection": {
    "windows": [
      "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Skyrim Special Edition"
    ],
    "linux": [
      "~/.steam/steam/steamapps/common/Skyrim Special Edition"
    ],
    "macos": [
      "~/Library/Application Support/Steam/steamapps/common/Skyrim Special Edition"
    ]
  },
  "tracked_paths": [
    "Data/",
    "*.ini",
    "Saves/"
  ],
  "ignore_patterns": [
    "*.log",
    "*.bak",
    "Logs/",
    "Crashes/"
  ],
  "large_file_warning_mb": 5000
}
```

---

### 4. Snapshot Data Structure (core/types/Snapshot.h)

```cpp
struct Snapshot {
    QString id;              // Git commit hash
    QString description;     // User-provided or auto-generated
    QDateTime timestamp;     // When snapshot was created
    QString author;          // Always "VGVC User" for now
    qint64 sizeBytes;       // Size of snapshot (if available)
    bool isAutomatic;        // Auto-snapshot vs manual
    
    // Convenience
    QString displayText() const {
        return QString("%1 - %2")
            .arg(timestamp.toString("yyyy-MM-dd HH:mm"))
            .arg(description);
    }
};
```

---

### 5. Result Type (core/types/Result.h)

**Use Rust-style Result<T, E> pattern:**

```cpp
template<typename T, typename E>
class Result {
public:
    static Result ok(T value) { return Result(std::move(value)); }
    static Result err(E error) { return Result(std::move(error)); }
    
    bool isOk() const { return m_isOk; }
    bool isErr() const { return !m_isOk; }
    
    T& value() { return m_value; }
    const T& value() const { return m_value; }
    
    E& error() { return m_error; }
    const E& error() const { return m_error; }
    
    T valueOr(T defaultValue) const {
        return m_isOk ? m_value : defaultValue;
    }
    
private:
    bool m_isOk;
    T m_value;
    E m_error;
    
    explicit Result(T value) : m_isOk(true), m_value(std::move(value)) {}
    explicit Result(E error) : m_isOk(false), m_error(std::move(error)) {}
};

// Specialization for void success
template<typename E>
class Result<void, E> {
public:
    static Result ok() { return Result(true); }
    static Result err(E error) { return Result(std::move(error)); }
    
    bool isOk() const { return m_isOk; }
    bool isErr() const { return !m_isOk; }
    
    E& error() { return m_error; }
    const E& error() const { return m_error; }
    
private:
    bool m_isOk;
    E m_error;
    
    explicit Result(bool ok) : m_isOk(ok) {}
    explicit Result(E error) : m_isOk(false), m_error(std::move(error)) {}
};
```

**Usage Example:**
```cpp
auto result = gitService->commit("My snapshot");
if (result.isOk()) {
    qDebug() << "Commit successful!";
} else {
    qWarning() << "Commit failed:" << result.error();
}
```

---

### 6. MainWindow (ui/MainWindow.h)

**Layout Structure:**
```
┌─────────────────────────────────────────────────┐
│ MenuBar: File | Edit | View | Help              │
├─────────────────────────────────────────────────┤
│ ToolBar: [Open Project ▼] [Settings ⚙]         │
├─────────────────────────────────────────────────┤
│                                                  │
│  ╔══════════════════════════════════════════╗  │
│  ║  RESTORE LAST WORKING SNAPSHOT           ║  │
│  ║  (Big, obvious, always enabled if > 0)   ║  │
│  ╚══════════════════════════════════════════╝  │
│                                                  │
│  Recent Snapshots                                │
│  ┌────────────────────────────────────────────┐ │
│  │ ✓ Working mod setup - 2 hours ago          │ │
│  │   Description: Before installing SkyUI     │ │
│  │   [Restore] [Delete]                       │ │
│  ├────────────────────────────────────────────┤ │
│  │   Fresh install - 1 day ago                │ │
│  │   [Restore] [Delete]                       │ │
│  └────────────────────────────────────────────┘ │
│                                                  │
│  [💾 Save New Snapshot]  [📋 Manage All]        │
│                                                  │
│  Status: Last sync: 5 minutes ago ☁            │
└─────────────────────────────────────────────────┘
```

**Key Widgets:**
- `QPushButton* m_restoreLastButton` - Large, primary action
- `QListView* m_snapshotList` - Uses SnapshotListModel
- `QPushButton* m_createSnapshotButton`
- `QPushButton* m_manageButton`
- `QLabel* m_statusLabel` - Shows repo size, sync status

**Signals to Connect:**
```cpp
// From UI to Manager
connect(m_createSnapshotButton, &QPushButton::clicked,
        this, &MainWindow::onCreateSnapshotClicked);
connect(m_restoreLastButton, &QPushButton::clicked,
        this, &MainWindow::onRestoreLastClicked);

// From Manager to UI
connect(m_snapshotManager, &SnapshotManager::snapshotCreated,
        this, &MainWindow::onSnapshotCreated);
connect(m_snapshotManager, &SnapshotManager::snapshotRestored,
        this, &MainWindow::onSnapshotRestored);
```

---

## Coding Standards

### C++ Style Guide

**General:**
- Follow Qt coding style (PascalCase classes, camelCase methods)
- Member variables prefixed with `m_`
- Private methods prefixed with nothing (just camelCase)
- Constants: `const` or `constexpr`, PascalCase: `MaxSnapshotCount`
- Use `auto` when type is obvious from context
- Prefer `nullptr` over `NULL`
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`) for ownership
- Qt objects use parent-child ownership, no smart pointers needed

**Header Files:**
```cpp
#ifndef GITSERVICE_H
#define GITSERVICE_H

#include <QObject>
#include <QFuture>
#include "types/Result.h"
#include "types/Snapshot.h"

class GitService : public QObject {
    Q_OBJECT
    
public:
    explicit GitService(const QString& repoPath, QObject* parent = nullptr);
    ~GitService() override = default;
    
    // Public interface...
    
signals:
    void operationProgress(int percentage, const QString& status);
    
private:
    // Private members...
    QString m_repoPath;
    
    // Private methods...
    Result<QString, QString> executeGitCommand(const QStringList& args)