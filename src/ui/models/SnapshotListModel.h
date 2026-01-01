#ifndef SNAPSHOTLISTMODEL_H
#define SNAPSHOTLISTMODEL_H

#include <QAbstractListModel>
#include "core/types/Snapshot.h"

/**
 * @brief Qt model for displaying snapshot list
 */
class SnapshotListModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    explicit SnapshotListModel(QObject* parent = nullptr);
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    
    void setSnapshots(const QList<Snapshot>& snapshots);
    Snapshot getSnapshot(int index) const;
    
private:
    QList<Snapshot> m_snapshots;
};

#endif // SNAPSHOTLISTMODEL_H
