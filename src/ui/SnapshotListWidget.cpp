#include "SnapshotListWidget.h"
#include <QVBoxLayout>

SnapshotListWidget::SnapshotListWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_model = new SnapshotListModel(this);
    m_listView = new QListView(this);
    m_listView->setModel(m_model);
    
    layout->addWidget(m_listView);
}

void SnapshotListWidget::setSnapshots(const QList<Snapshot>& snapshots)
{
    m_model->setSnapshots(snapshots);
}
