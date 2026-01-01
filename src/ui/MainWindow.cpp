#include "MainWindow.h"
#include "CreateSnapshotDialog.h"
#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QProgressDialog>
#include <QFutureWatcher>
#include "utils/FileUtils.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_gitService(nullptr)
    , m_snapshotManager(nullptr)
    , m_presetManager(new PresetManager(this))
{
    setupUi();
    setupConnections();
}

void MainWindow::setupUi()
{
    setWindowTitle("Video Game Version Control");
    resize(800, 600);
    
    // Create central widget
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create menu bar
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&Open Project...", this, &MainWindow::onOpenProjectClicked);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close);
    
    QMenu* viewMenu = menuBar->addMenu("&View");
    viewMenu->addAction("&Refresh", this, &MainWindow::refreshSnapshotList);
    
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", [this]() {
        QMessageBox::about(this, "About VGVC",
            "Video Game Version Control\n\n"
            "A tool for managing game saves and mods with ease.");
    });
    
    setMenuBar(menuBar);
    
    // Create toolbar
    QToolBar* toolBar = new QToolBar(this);
    m_settingsButton = new QPushButton("⚙ Settings", this);
    toolBar->addWidget(m_settingsButton);
    addToolBar(toolBar);
    
    // Restore last snapshot button (large, prominent)
    m_restoreLastButton = new QPushButton("⟲ RESTORE LAST WORKING SNAPSHOT", this);
    m_restoreLastButton->setMinimumHeight(60);
    m_restoreLastButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #4CAF50;"
        "  color: white;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #45a049;"
        "}"
    );
    m_restoreLastButton->setEnabled(false);
    mainLayout->addWidget(m_restoreLastButton);
    
    mainLayout->addSpacing(20);
    
    // Recent snapshots label
    QLabel* recentLabel = new QLabel("Recent Snapshots", this);
    recentLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    mainLayout->addWidget(recentLabel);
    
    // Snapshot list with empty state label
    QWidget* listContainer = new QWidget(this);
    QVBoxLayout* listLayout = new QVBoxLayout(listContainer);
    listLayout->setContentsMargins(0, 0, 0, 0);
    
    m_snapshotList = new QListView(this);
    m_snapshotModel = new SnapshotListModel(this);
    m_snapshotList->setModel(m_snapshotModel);
    
    m_emptyListLabel = new QLabel(this);
    m_emptyListLabel->setAlignment(Qt::AlignCenter);
    m_emptyListLabel->setWordWrap(true);
    m_emptyListLabel->setStyleSheet(
        "QLabel {"
        "  color: #666;"
        "  font-size: 14px;"
        "  padding: 40px;"
        "}"
    );
    m_emptyListLabel->setText("No Game Loaded!\nOpen a game folder to manage it.");
    
    listLayout->addWidget(m_snapshotList);
    listLayout->addWidget(m_emptyListLabel);
    
    mainLayout->addWidget(listContainer);
    
    // Action buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_createSnapshotButton = new QPushButton("💾 Save New Snapshot", this);
    m_manageButton = new QPushButton("📋 Manage All", this);
    buttonLayout->addWidget(m_createSnapshotButton);
    buttonLayout->addWidget(m_manageButton);
    mainLayout->addLayout(buttonLayout);
    
    // Status label
    m_statusLabel = new QLabel("No project loaded", this);
    mainLayout->addWidget(m_statusLabel);
    
    setCentralWidget(centralWidget);
    
    // Status bar
    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    connect(m_createSnapshotButton, &QPushButton::clicked,
            this, &MainWindow::onCreateSnapshotClicked);
    connect(m_restoreLastButton, &QPushButton::clicked,
            this, &MainWindow::onRestoreLastClicked);
    connect(m_manageButton, &QPushButton::clicked,
            this, &MainWindow::onManageClicked);
    connect(m_settingsButton, &QPushButton::clicked,
            this, &MainWindow::onSettingsClicked);
}

void MainWindow::onOpenProjectClicked()
{
    QString path = QFileDialog::getExistingDirectory(
        this,
        "Select Game Directory",
        QDir::homePath()
    );
    
    if (path.isEmpty()) {
        return;
    }
    
    m_currentProjectPath = path;
    
    // Initialize git service
    if (m_gitService) {
        m_gitService->deleteLater();
        m_snapshotManager->deleteLater();
    }
    
    m_gitService = new GitService(path, this);
    m_snapshotManager = new SnapshotManager(m_gitService, this);
    
    connect(m_snapshotManager, &SnapshotManager::snapshotCreated,
            this, &MainWindow::onSnapshotCreated);
    connect(m_snapshotManager, &SnapshotManager::snapshotRestored,
            this, &MainWindow::onSnapshotRestored);
    connect(m_snapshotManager, &SnapshotManager::operationProgress,
            this, &MainWindow::onOperationProgress);
    
    // Check if git repo exists
    QDir repoDir(path);
    if (repoDir.exists(".git")) {
        // Repo exists, load snapshots
        refreshSnapshotList();
    } else {
        // No git repo yet - don't initialize until user is ready
        m_statusLabel->setText("No git repository found. Create a snapshot to initialize.");
        m_snapshotModel->setSnapshots(QList<Snapshot>());
        m_emptyListLabel->setText("No Checkpoints Found!\nCreate a new checkpoint using the buttons below");
        m_emptyListLabel->setVisible(true);
        m_snapshotList->setVisible(false);
        m_restoreLastButton->setEnabled(false);
    }
    
    updateStatusBar();
}

void MainWindow::onCreateSnapshotClicked()
{
    if (!m_snapshotManager) {
        QMessageBox::warning(this, "No Project", 
            "Please open a project first.");
        return;
    }
    
    CreateSnapshotDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString description = dialog.getDescription();
        
        auto* progress = new QProgressDialog("Creating snapshot...", "Cancel", 0, 100, this);
        progress->setWindowModality(Qt::WindowModal);
        progress->show();
        
        auto* watcher = new QFutureWatcher<Result<void, QString>>(this);
        connect(watcher, &QFutureWatcher<Result<void, QString>>::finished,
                this, [this, watcher, progress]() {
            progress->close();
            auto result = watcher->result();
            if (result.isErr()) {
                QMessageBox::critical(this, "Error", 
                    QString("Failed to create snapshot: %1").arg(result.error()));
            } else {
                refreshSnapshotList();
            }
            watcher->deleteLater();
            progress->deleteLater();
        });
        
        connect(m_snapshotManager, &SnapshotManager::operationProgress,
                progress, &QProgressDialog::setValue);
        
        QFuture<Result<void, QString>> future = m_snapshotManager->createSnapshot(description);
        watcher->setFuture(future);
    }
}

void MainWindow::onRestoreLastClicked()
{
    if (!m_snapshotManager) {
        return;
    }
    
    // Get the latest snapshot asynchronously
    auto* listWatcher = new QFutureWatcher<Result<QList<Snapshot>, QString>>(this);
    connect(listWatcher, &QFutureWatcher<Result<QList<Snapshot>, QString>>::finished,
            this, [this, listWatcher]() {
        auto result = listWatcher->result();
        listWatcher->deleteLater();
        
        if (result.isErr() || result.value().isEmpty()) {
            QMessageBox::warning(this, "No Snapshots",
                "No snapshots available to restore.");
            return;
        }
        
        const Snapshot& latest = result.value().first();
        
        int ret = QMessageBox::question(this, "Restore Snapshot",
            QString("Restore snapshot: %1\n\nThis will revert your game files to this state.")
                .arg(latest.displayText()));
        
        if (ret == QMessageBox::Yes) {
            auto* progress = new QProgressDialog("Restoring snapshot...", "Cancel", 0, 100, this);
            progress->setWindowModality(Qt::WindowModal);
            progress->show();
            
            auto* restoreWatcher = new QFutureWatcher<Result<void, QString>>(this);
            connect(restoreWatcher, &QFutureWatcher<Result<void, QString>>::finished,
                    this, [this, restoreWatcher, progress]() {
                progress->close();
                auto restoreResult = restoreWatcher->result();
                if (restoreResult.isErr()) {
                    QMessageBox::critical(this, "Error",
                        QString("Failed to restore: %1").arg(restoreResult.error()));
                } else {
                    QMessageBox::information(this, "Success",
                        "Snapshot restored successfully!");
                    refreshSnapshotList();
                }
                restoreWatcher->deleteLater();
                progress->deleteLater();
            });
            
            connect(m_snapshotManager, &SnapshotManager::operationProgress,
                    progress, &QProgressDialog::setValue);
            
            QFuture<Result<void, QString>> restoreFuture = m_snapshotManager->restoreSnapshot(latest.id);
            restoreWatcher->setFuture(restoreFuture);
        }
    });
    
    QFuture<Result<QList<Snapshot>, QString>> future = m_snapshotManager->listSnapshots();
    listWatcher->setFuture(future);
}

void MainWindow::onManageClicked()
{
    // TODO: Open snapshot management dialog
    QMessageBox::information(this, "Not Implemented",
        "Snapshot management dialog coming soon!");
}

void MainWindow::onSettingsClicked()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onSnapshotCreated(const Snapshot& snapshot)
{
    refreshSnapshotList();
    statusBar()->showMessage(QString("Snapshot created: %1").arg(snapshot.description), 3000);
}

void MainWindow::onSnapshotRestored(const QString& snapshotId)
{
    refreshSnapshotList();
}

void MainWindow::onOperationProgress(int percentage, const QString& status)
{
    statusBar()->showMessage(QString("%1 (%2%)").arg(status).arg(percentage));
}

void MainWindow::refreshSnapshotList()
{
    if (!m_snapshotManager) {
        return;
    }
    
    auto* watcher = new QFutureWatcher<Result<QList<Snapshot>, QString>>(this);
    connect(watcher, &QFutureWatcher<Result<QList<Snapshot>, QString>>::finished,
            this, [this, watcher]() {
        auto result = watcher->result();
        
        if (result.isOk()) {
            if (result.value().isEmpty()) {
                m_statusLabel->setText("No snapshots yet. Create your first one!");
                m_snapshotModel->setSnapshots(QList<Snapshot>());
                m_emptyListLabel->setText("No Checkpoints Found!\nCreate a new checkpoint using the buttons below");
                m_emptyListLabel->setVisible(true);
                m_snapshotList->setVisible(false);
            } else {
                m_snapshotModel->setSnapshots(result.value());
                m_statusLabel->setText(QString("Project: %1").arg(m_currentProjectPath));
                m_emptyListLabel->setVisible(false);
                m_snapshotList->setVisible(true);
            }
            m_restoreLastButton->setEnabled(!result.value().isEmpty());
        } else {
            m_statusLabel->setText(QString("Error loading snapshots: %1").arg(result.error()));
            m_restoreLastButton->setEnabled(false);
            m_emptyListLabel->setText("No Checkpoints Found!\nCreate a new checkpoint using the buttons below");
            m_emptyListLabel->setVisible(true);
            m_snapshotList->setVisible(false);
        }
        
        watcher->deleteLater();
    });
    
    QFuture<Result<QList<Snapshot>, QString>> future = m_snapshotManager->listSnapshots();
    watcher->setFuture(future);
}

void MainWindow::updateStatusBar()
{
    if (m_currentProjectPath.isEmpty()) {
        m_statusLabel->setText("No project loaded");
    } else {
        m_statusLabel->setText(QString("Project: %1").arg(m_currentProjectPath));
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // TODO: Create auto-snapshot if enabled in settings
    QMainWindow::closeEvent(event);
}
