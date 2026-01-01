#include "SnapshotListModel.h"

SnapshotListModel::SnapshotListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int SnapshotListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_snapshots.count();
}

QVariant SnapshotListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_snapshots.count()) {
        return QVariant();
    }
    
    const Snapshot& snapshot = m_snapshots.at(index.row());
    
    switch (role) {
        case Qt::DisplayRole:
            return snapshot.displayText();
            
        case Qt::ToolTipRole:
            return QString("ID: %1\nAuthor: %2\nAutomatic: %3")
                .arg(snapshot.id)
                .arg(snapshot.author)
                .arg(snapshot.isAutomatic ? "Yes" : "No");
            
        default:
            return QVariant();
    }
}

void SnapshotListModel::setSnapshots(const QList<Snapshot>& snapshots)
{
    beginResetModel();
    m_snapshots = snapshots;
    endResetModel();
}

Snapshot SnapshotListModel::getSnapshot(int index) const
{
    if (index >= 0 && index < m_snapshots.count()) {
        return m_snapshots.at(index);
    }
    return Snapshot();
}
