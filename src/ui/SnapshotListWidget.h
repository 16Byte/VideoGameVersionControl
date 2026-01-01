#ifndef SNAPSHOTLISTWIDGET_H
#define SNAPSHOTLISTWIDGET_H

#include <QWidget>
#include <QListView>
#include "core/types/Snapshot.h"
#include "ui/models/SnapshotListModel.h"

/**
 * @brief Widget for displaying and interacting with snapshot list
 */
class SnapshotListWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit SnapshotListWidget(QWidget* parent = nullptr);
    
    void setSnapshots(const QList<Snapshot>& snapshots);
    
signals:
    void restoreRequested(const QString& snapshotId);
    void deleteRequested(const QString& snapshotId);
    
private:
    QListView* m_listView;
    SnapshotListModel* m_model;
};

#endif // SNAPSHOTLISTWIDGET_H
