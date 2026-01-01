#ifndef CREATESNAPSHOTDIALOG_H
#define CREATESNAPSHOTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>

/**
 * @brief Dialog for creating a new snapshot
 */
class CreateSnapshotDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit CreateSnapshotDialog(QWidget* parent = nullptr);
    
    QString getDescription() const;
    
private:
    QLineEdit* m_descriptionEdit;
    QTextEdit* m_notesEdit;
};

#endif // CREATESNAPSHOTDIALOG_H
