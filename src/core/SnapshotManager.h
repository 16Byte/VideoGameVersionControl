#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <QObject>
#include <QFuture>
#include "GitService.h"
#include "types/Result.h"
#include "types/Snapshot.h"

/**
 * @brief High-level snapshot operations
 * 
 * Provides user-facing snapshot functionality built on top of GitService.
 * Handles automatic snapshot descriptions, safety backups, etc.
 */
class SnapshotManager : public QObject {
    Q_OBJECT
    
public:
    explicit SnapshotManager(GitService* gitService, QObject* parent = nullptr);
    ~SnapshotManager() override = default;
    
    QFuture<Result<void, QString>> createSnapshot(const QString& description);
    QFuture<Result<QList<Snapshot>, QString>> listSnapshots();
    QFuture<Result<void, QString>> restoreSnapshot(const QString& snapshotId);
    QFuture<Result<void, QString>> deleteSnapshot(const QString& snapshotId);
    
signals:
    void snapshotCreated(const Snapshot& snapshot);
    void snapshotRestored(const QString& snapshotId);
    void operationProgress(int percentage, const QString& status);
    
private:
    GitService* m_gitService;
    
    QString generateDescription(const QString& userDescription);
};

#endif // SNAPSHOTMANAGER_H
