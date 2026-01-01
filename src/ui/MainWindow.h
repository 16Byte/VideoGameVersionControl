#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QListView>
#include <QLabel>
#include "core/SnapshotManager.h"
#include "core/PresetManager.h"
#include "ui/models/SnapshotListModel.h"

/**
 * @brief Main application window
 * 
 * Provides the primary UI for snapshot management.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    void onCreateSnapshotClicked();
    void onRestoreLastClicked();
    void onManageClicked();
    void onSettingsClicked();
    void onOpenProjectClicked();
    
    void onSnapshotCreated(const Snapshot& snapshot);
    void onSnapshotRestored(const QString& snapshotId);
    void onOperationProgress(int percentage, const QString& status);
    
private:
    void setupUi();
    void setupConnections();
    void refreshSnapshotList();
    void updateStatusBar();
    
    // Core services
    GitService* m_gitService;
    SnapshotManager* m_snapshotManager;
    PresetManager* m_presetManager;
    
    // UI widgets
    QPushButton* m_restoreLastButton;
    QPushButton* m_createSnapshotButton;
    QPushButton* m_manageButton;
    QPushButton* m_settingsButton;
    QListView* m_snapshotList;
    SnapshotListModel* m_snapshotModel;
    QLabel* m_statusLabel;
    QLabel* m_emptyListLabel;
    
    // State
    QString m_currentProjectPath;
};

#endif // MAINWINDOW_H
