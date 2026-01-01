#ifndef GITSERVICE_H
#define GITSERVICE_H

#include <QObject>
#include <QFuture>
#include <QString>
#include <QStringList>
#include "types/Result.h"
#include "types/Snapshot.h"

/**
 * @brief Low-level Git operations wrapper
 * 
 * Executes Git commands via QProcess and parses output.
 * All operations are async and return QFuture<Result<T, QString>>.
 */
class GitService : public QObject {
    Q_OBJECT
    
public:
    explicit GitService(const QString& repoPath, QObject* parent = nullptr);
    ~GitService() override = default;
    
    // Async operations
    QFuture<Result<void, QString>> init();
    QFuture<Result<void, QString>> commit(const QString& message);
    QFuture<Result<QList<Snapshot>, QString>> getHistory(int limit = 50);
    QFuture<Result<void, QString>> checkout(const QString& commitHash);
    QFuture<Result<qint64, QString>> getRepoSize();
    QFuture<Result<bool, QString>> hasChanges();
    
signals:
    void operationProgress(int percentage, const QString& status);
    
private:
    QString m_repoPath;
    QString m_gitExecutable;  // Path to git binary
    
    Result<QString, QString> executeGitCommand(const QStringList& args);
    Result<Snapshot, QString> parseCommitLog(const QString& logLine);
    QString findGitExecutable();
};

#endif // GITSERVICE_H
