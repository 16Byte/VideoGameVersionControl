#ifndef RESTOREDIALOG_H
#define RESTOREDIALOG_H

#include <QDialog>
#include "core/types/Snapshot.h"

/**
 * @brief Dialog for restoring a snapshot with preview
 */
class RestoreDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit RestoreDialog(const Snapshot& snapshot, QWidget* parent = nullptr);
    
private:
    Snapshot m_snapshot;
};

#endif // RESTOREDIALOG_H
