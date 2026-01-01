#include "SnapshotManager.h"
#include <QDateTime>
#include <QtConcurrent>

SnapshotManager::SnapshotManager(GitService* gitService, QObject* parent)
    : QObject(parent)
    , m_gitService(gitService)
{
}

QString SnapshotManager::generateDescription(const QString& userDescription)
{
    if (!userDescription.isEmpty()) {
        return userDescription;
    }
    
    // Auto-generate: "Snapshot - YYYY-MM-DD HH:MM"
    return QString("Snapshot - %1").arg(
        QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm")
    );
}

QFuture<Result<void, QString>> SnapshotManager::createSnapshot(const QString& description)
{
    return QtConcurrent::run([this, description]() -> Result<void, QString> {
        emit operationProgress(25, "Preparing snapshot...");
        
        QString finalDescription = generateDescription(description);
        
        emit operationProgress(50, "Creating snapshot...");
        
        auto commitFuture = m_gitService->commit(finalDescription);
        commitFuture.waitForFinished();
        auto result = commitFuture.result();
        
        if (result.isErr()) {
            return Result<void, QString>::err(result.error());
        }
        
        emit operationProgress(100, "Snapshot created");
        
        // Fetch the snapshot to emit signal
        auto historyFuture = m_gitService->getHistory(1);
        historyFuture.waitForFinished();
        auto historyResult = historyFuture.result();
        
        if (historyResult.isOk() && !historyResult.value().isEmpty()) {
            emit snapshotCreated(historyResult.value().first());
        }
        
        return Result<void, QString>::ok();
    });
}

QFuture<Result<QList<Snapshot>, QString>> SnapshotManager::listSnapshots()
{
    return m_gitService->getHistory();
}

QFuture<Result<void, QString>> SnapshotManager::restoreSnapshot(const QString& snapshotId)
{
    return QtConcurrent::run([this, snapshotId]() -> Result<void, QString> {
        emit operationProgress(20, "Creating safety backup...");
        
        // Create automatic safety snapshot before restoring
        auto hasChangesFuture = m_gitService->hasChanges();
        hasChangesFuture.waitForFinished();
        auto hasChangesResult = hasChangesFuture.result();
        
        if (hasChangesResult.isOk() && hasChangesResult.value()) {
            auto safetyFuture = m_gitService->commit("[AUTO] Safety backup before restore");
            safetyFuture.waitForFinished();
            // Continue even if safety backup fails
        }
        
        emit operationProgress(50, "Restoring snapshot...");
        
        auto checkoutFuture = m_gitService->checkout(snapshotId);
        checkoutFuture.waitForFinished();
        auto result = checkoutFuture.result();
        
        if (result.isErr()) {
            return Result<void, QString>::err(result.error());
        }
        
        emit operationProgress(100, "Snapshot restored");
        emit snapshotRestored(snapshotId);
        
        return Result<void, QString>::ok();
    });
}

QFuture<Result<void, QString>> SnapshotManager::deleteSnapshot(const QString& snapshotId)
{
    // TODO: Implement snapshot deletion
    // This requires more complex git operations (rebase or filter-branch)
    // For MVP, we might want to skip this feature
    return QtConcurrent::run([snapshotId]() -> Result<void, QString> {
        return Result<void, QString>::err("Snapshot deletion not yet implemented");
    });
}
