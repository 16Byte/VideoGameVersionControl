#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>

/**
 * @brief Application settings dialog
 */
class SettingsDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    
private:
    void loadSettings();
    void saveSettings();
    
    QCheckBox* m_autoSnapshotCheck;
    QSpinBox* m_maxSnapshotsSpin;
};

#endif // SETTINGSDIALOG_H
