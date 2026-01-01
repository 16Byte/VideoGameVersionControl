#include "GitService.h"
#include <QProcess>
#include <QDir>
#include <QStandardPaths>
#include <QtConcurrent>

GitService::GitService(const QString& repoPath, QObject* parent)
    : QObject(parent)
    , m_repoPath(repoPath)
    , m_gitExecutable(findGitExecutable())
{
}

QString GitService::findGitExecutable()
{
    // Try to find git in PATH
    QString git = QStandardPaths::findExecutable("git");
    if (!git.isEmpty()) {
        return git;
    }
    
    // Platform-specific fallbacks
#ifdef Q_OS_WIN
    // Check common Windows Git installation paths
    QStringList windowsPaths = {
        "C:/Program Files/Git/bin/git.exe",
        "C:/Program Files (x86)/Git/bin/git.exe"
    };
    for (const QString& path : windowsPaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }
#endif
    
    return "git";  // Fallback, might not work
}

Result<QString, QString> GitService::executeGitCommand(const QStringList& args)
{
    QProcess process;
    process.setWorkingDirectory(m_repoPath);
    process.setProgram(m_gitExecutable);
    process.setArguments(args);
    
    process.start();
    if (!process.waitForStarted(5000)) {
        return Result<QString, QString>::err("Failed to start git process");
    }
    
    if (!process.waitForFinished(30000)) {  // 30 second timeout
        process.kill();
        return Result<QString, QString>::err("Git operation timed out");
    }
    
    if (process.exitCode() != 0) {
        QString error = process.readAllStandardError();
        return Result<QString, QString>::err(QString("Git error: %1").arg(error));
    }
    
    QString output = process.readAllStandardOutput();
    return Result<QString, QString>::ok(output);
}

QFuture<Result<void, QString>> GitService::init()
{
    return QtConcurrent::run([this]() -> Result<void, QString> {
        // Initialize git repository
        auto initResult = executeGitCommand({"init", "-b", "main"});
        if (initResult.isErr()) {
            return Result<void, QString>::err(initResult.error());
        }
        
        // Set user name
        auto nameResult = executeGitCommand({"config", "user.name", "VGVC User"});
        if (nameResult.isErr()) {
            return Result<void, QString>::err(nameResult.error());
        }
        
        // Set user email
        auto emailResult = executeGitCommand({"config", "user.email", "vgvc@local"});
        if (emailResult.isErr()) {
            return Result<void, QString>::err(emailResult.error());
        }
        
        return Result<void, QString>::ok();
    });
}

QFuture<Result<void, QString>> GitService::commit(const QString& message)
{
    return QtConcurrent::run([this, message]() -> Result<void, QString> {
        // Add all files
        auto addResult = executeGitCommand({"add", "."});
        if (addResult.isErr()) {
            return Result<void, QString>::err(addResult.error());
        }
        
        // Commit
        auto commitResult = executeGitCommand({"commit", "-m", message});
        if (commitResult.isErr()) {
            return Result<void, QString>::err(commitResult.error());
        }
        
        return Result<void, QString>::ok();
    });
}

QFuture<Result<QList<Snapshot>, QString>> GitService::getHistory(int limit)
{
    return QtConcurrent::run([this, limit]() -> Result<QList<Snapshot>, QString> {
        // Ensure we're on main branch
        auto checkoutResult = executeGitCommand({"checkout", "main"});
        if (checkoutResult.isErr()) {
            return Result<QList<Snapshot>, QString>::err(checkoutResult.error());
        }
        
        // Format: hash|author|timestamp|subject
        auto result = executeGitCommand({
            "log",
            "--pretty=format:%H|%an|%at|%s",
            QString("-n%1").arg(limit)
        });
        
        if (result.isErr()) {
            return Result<QList<Snapshot>, QString>::err(result.error());
        }
        
        QList<Snapshot> snapshots;
        QStringList lines = result.value().split('\n', Qt::SkipEmptyParts);
        
        for (int i = 0; i < lines.size(); ++i) {
            const QString& line = lines[i];
            
            // Skip the very last commit (oldest/initial commit from init)
            if (i == lines.size() - 1 && lines.size() > 1) {
                continue;
            }
            
            auto snapshotResult = parseCommitLog(line);
            if (snapshotResult.isOk()) {
                snapshots.append(snapshotResult.value());
            }
        }
        
        return Result<QList<Snapshot>, QString>::ok(snapshots);
    });
}

Result<Snapshot, QString> GitService::parseCommitLog(const QString& logLine)
{
    QStringList parts = logLine.split('|');
    if (parts.size() < 4) {
        return Result<Snapshot, QString>::err("Invalid log format");
    }
    
    Snapshot snapshot;
    snapshot.id = parts[0];
    snapshot.author = parts[1];
    snapshot.timestamp = QDateTime::fromSecsSinceEpoch(parts[2].toLongLong());
    snapshot.description = parts[3];
    snapshot.isAutomatic = snapshot.description.startsWith("[AUTO]");
    
    return Result<Snapshot, QString>::ok(snapshot);
}

QFuture<Result<void, QString>> GitService::checkout(const QString& commitHash)
{
    return QtConcurrent::run([this, commitHash]() -> Result<void, QString> {
        auto result = executeGitCommand({"checkout", commitHash});
        if (result.isErr()) {
            return Result<void, QString>::err(result.error());
        }
        return Result<void, QString>::ok();
    });
}

QFuture<Result<qint64, QString>> GitService::getRepoSize()
{
    return QtConcurrent::run([this]() -> Result<qint64, QString> {
        auto result = executeGitCommand({"count-objects", "-v"});
        if (result.isErr()) {
            return Result<qint64, QString>::err(result.error());
        }
        
        // Parse size from output (simplified - just return 0 for now)
        // TODO: Implement proper parsing of git count-objects output
        return Result<qint64, QString>::ok(0);
    });
}

QFuture<Result<bool, QString>> GitService::hasChanges()
{
    return QtConcurrent::run([this]() -> Result<bool, QString> {
        auto result = executeGitCommand({"status", "--porcelain"});
        if (result.isErr()) {
            return Result<bool, QString>::err(result.error());
        }
        
        bool hasChanges = !result.value().trimmed().isEmpty();
        return Result<bool, QString>::ok(hasChanges);
    });
}
