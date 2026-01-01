# VideoGameVersionControl

## Design Brief v2.0

### Summary

VideoGameVersionControl is a native desktop application that helps players preserve and restore their game states—including saves, mods, and settings. It uses Git internally but hides all complexity behind a simple "snapshot and restore" interface. Cloud backup is optional and designed to be frictionless.

### Target Audience

- Modders who've broken their game and need to roll back
- Players switching between machines who want their setup everywhere
- Communities sharing curated mod packs
- Anyone who's ever lost 100 hours of progress to a corrupted save

### Core Philosophy

**Local first.** The app must work perfectly without any cloud service. Cloud sync is a convenience feature, not a requirement.

**Zero Git knowledge required.** Users should never see commits, branches, or merge conflicts.

**Instant "oh shit" recovery.** One click to restore the last working state.

---

## Key Features

### 1. Game Project Management

**Initialize a Project:**
- Point at game installation folder or save directory
- Auto-detect game type if possible (check registry, Steam library, common paths)
- Apply curated template for tracked files and ignore patterns
- Manual fallback for unknown games

**Tracked Content:**
- Save files
- Configuration files
- Mod files and load orders
- Shader caches (optional, user choice)

### 2. Snapshot System

**Creating Snapshots:**
- Big green "Save Snapshot" button on main screen
- Optional description field (auto-generates timestamp if left empty)
- Progress indicator for large directories
- Automatic background snapshots on app close (optional setting)

**Restoring Snapshots:**
- List view showing all snapshots with timestamps and descriptions
- Preview what files will change before restoring
- One-click restore with automatic backup of current state
- "Restore Last Snapshot" always visible on main screen

**Snapshot Management:**
- Delete old snapshots to save space
- Export snapshot as shareable archive
- Prune system to keep last N snapshots automatically

### 3. Cloud Sync (Optional Feature)

**Phase 1: Simple Backend (MVP)**
- Use GitHub OAuth (no SSH keys, no terminal commands)
- Create private repo automatically via GitHub API
- Handle push/pull transparently
- Simple conflict resolution: "Keep Local" or "Download Cloud" buttons

**Phase 2: Better Storage (Post-MVP)**
- Integrate with Git LFS for large files
- Or switch to purpose-built backend (S3-compatible storage)
- Support multiple cloud providers (GitHub, GitLab, self-hosted)

**User Experience:**
- "Enable Cloud Backup" button walks through OAuth in default browser
- Show sync status indicator (synced, syncing, conflicts)
- Manual sync controls + optional auto-sync
- Works offline gracefully—queue operations for later

### 4. Sharing & Community

**Export Features:**
- Export snapshot as compressed archive (.zip or .tar.gz)
- Include manifest file listing mods and versions
- Option to exclude save files (share setup only)

**Import Features:**
- Drag-and-drop archive to import
- Preview contents before applying
- Warn about conflicts with existing files

**Mod Licensing Awareness:**
- Flag known problematic mods that can't be redistributed
- Provide mod list + download links instead of files when appropriate

### 5. Smart Storage Management

**Size Optimization:**
- Warn when repo exceeds 5GB (GitHub soft limit)
- Compress large binary files
- Deduplicate identical files across snapshots
- Prune old snapshots automatically based on policy

**Size Warnings:**
- Show current repository size in UI
- Estimate before creating snapshot
- Suggest cleanup when approaching limits

---

## Technical Architecture

### Language & Framework
- **C++17** (minimum) or **C++20** (preferred)
- **Qt 6.5+** for cross-platform GUI and core utilities

### Version Control Backend
**Option A: Git CLI** (Recommended for MVP)
- Spawn `git` commands via `QProcess`
- Parse output with regex/string processing
- Simpler, less code to maintain
- Requires Git installed (bundle portable Git on Windows)

**Option B: libgit2**
- More control, no external dependencies
- More complex integration
- Better for advanced features post-MVP

### Cloud Integration
- **Qt Network** for HTTP requests (GitHub API, OAuth)
- **QOAuth2AuthorizationCodeFlow** for GitHub OAuth
- Fallback to `libcurl` if needed for specific APIs

### Storage Format
- Standard Git repository structure
- `.vgvc/` directory for app metadata:
  - `config.json` - app settings
  - `presets.json` - game-specific templates
  - `manifest.json` - snapshot metadata (descriptions, tags)

### Build System
- **CMake 3.20+**
- Modular structure for easy platform-specific code
- Separate packaging scripts per platform

---

## Platform Support

### Windows
- Native Qt 6 application
- Packaging: MSIX (Microsoft Store) or Inno Setup installer
- Bundle portable Git if needed
- Support Windows 10 22H2+ and Windows 11

### macOS
- Native Qt 6 application
- Packaging: DMG with drag-to-Applications
- Notarized for Gatekeeper
- Support macOS 12 (Monterey) and newer
- Universal binary (Intel + Apple Silicon)

### Linux
- Primary: **Flatpak** (distribute via Flathub)
- Secondary: **AppImage** for distro-agnostic portability
- Optional: Snap package
- Support major DEs (GNOME, KDE Plasma, XFCE)

---

## Game Presets System

### Preset Format (JSON)
```json
{
  "game_id": "kerbal_space_program",
  "display_name": "Kerbal Space Program",
  "detection": {
    "windows": [
      "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Kerbal Space Program"
    ],
    "linux": [
      "~/.steam/steam/steamapps/common/Kerbal Space Program"
    ],
    "macos": [
      "~/Library/Application Support/Steam/steamapps/common/Kerbal Space Program"
    ]
  },
  "tracked_paths": [
    "saves/",
    "GameData/",
    "settings.cfg"
  ],
  "ignore_patterns": [
    "*.log",
    "*.tmp",
    "Screenshots/",
    "Crashes/"
  ],
  "large_file_warning_mb": 5000
}
```

### Built-in Presets (Launch)
- Kerbal Space Program
- Minecraft (Java Edition)
- The Elder Scrolls V: Skyrim
- Fallout 4
- Stardew Valley
- Terraria

### User-Created Presets
- Users can save custom configurations
- Export/import preset files
- Community preset repository (future)

---

## UI/UX Design Principles

### Main Window Layout
```
+--------------------------------------------------+
|  [Game: Skyrim SE ▼]           [☁ Synced] [⚙]  |
+--------------------------------------------------+
|                                                  |
|  [HUGE RESTORE LAST SNAPSHOT BUTTON]            |
|                                                  |
|  Recent Snapshots:                               |
|  ┌────────────────────────────────────────────┐ |
|  │ ✓ Working mod setup - 2 hours ago          │ |
|  │   Before installing SkyUI 5.2               │ |
|  ├────────────────────────────────────────────┤ |
|  │   Fresh install - 1 day ago                │ |
|  ├────────────────────────────────────────────┤ |
|  │   100% completion - 3 days ago             │ |
|  └────────────────────────────────────────────┘ |
|                                                  |
|  [Save New Snapshot]  [Manage All Snapshots]    |
|                                                  |
+--------------------------------------------------+
```

### Design Guidelines
- **No Git terminology** in the UI (no "commit", "push", "pull")
- Use gamer-friendly terms: "Snapshot", "Restore", "Backup"
- Large, obvious buttons for primary actions
- Progress indicators for long operations
- Native file dialogs and system integration
- Dark mode support (many gamers prefer it)

### Error Handling
- Clear, actionable error messages
- No stack traces or technical jargon
- Suggest fixes when possible
- Automatic error reporting (opt-in)

---

## MVP Feature Set (Version 1.0)

**Must Have:**
- Initialize project for a game folder
- Create snapshots with descriptions
- List and restore snapshots
- Delete snapshots
- 3-5 built-in game presets
- Local-only operation (no cloud)
- Basic Git CLI integration

**Should Have:**
- Auto-detect game installations
- Size warnings and management
- Export/import snapshots
- Settings panel (auto-snapshot on close, prune policies)

**Won't Have (Yet):**
- Cloud sync (v1.1)
- Community features (v1.2+)
- Advanced Git features (branches, tags)
- Mod conflict detection

---

## Post-MVP Roadmap

### Version 1.1 - Cloud Sync
- GitHub OAuth integration
- Automatic push/pull
- Simple conflict resolution
- Sync status indicators

### Version 1.2 - Sharing
- Export optimized archives
- Import community snapshots
- Mod manifest generation
- Link to mod sources when redistribution isn't allowed

### Version 1.3 - Power User Features
- Multiple games in one workspace
- Snapshot tagging and search
- Diff viewer for config files
- Custom ignore patterns UI

### Version 2.0 - Advanced
- Git LFS integration
- Self-hosted backend option
- Scheduled snapshots
- Steam Workshop integration

---

## Development Priorities

1. **Get local snapshots working perfectly** - This is the core value
2. **Polish the restore experience** - It must be bulletproof
3. **Make it fast** - Large mod folders need good performance
4. **Add cloud as an optional extra** - Don't let it complicate v1
5. **Community features come last** - Focus on single-user experience first

---

## Success Metrics

- User can go from install to first snapshot in < 2 minutes
- Restore operation completes in < 30 seconds for typical mod setup
- Zero GitHub/Git knowledge required for basic use
- Works offline for all core features
- Supports 90% of popular moddable games via presets

---

## Open Questions

1. Bundle Git binary or require system installation?
   - **Recommendation:** Bundle on Windows/macOS, require on Linux
2. Maximum recommended repository size?
   - **Recommendation:** Warn at 5GB, hard limit at 10GB, suggest cleanup
3. Default snapshot retention policy?
   - **Recommendation:** Keep all manual snapshots, auto-prune auto-snapshots older than 30 days
4. Should we support non-Steam games?
   - **Yes**, but Steam detection is priority for MVP