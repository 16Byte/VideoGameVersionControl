#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QGroupBox>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    resize(450, 350);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // General settings group
    QGroupBox* generalGroup = new QGroupBox("General", this);
    QFormLayout* generalLayout = new QFormLayout(generalGroup);
    
    m_autoSnapshotCheck = new QCheckBox("Create automatic snapshot on application close", this);
    m_autoSnapshotCheck->setChecked(true);
    generalLayout->addRow(m_autoSnapshotCheck);
    
    m_maxSnapshotsSpin = new QSpinBox(this);
    m_maxSnapshotsSpin->setRange(1, 1000);
    m_maxSnapshotsSpin->setValue(50);
    m_maxSnapshotsSpin->setSuffix(" snapshots");
    generalLayout->addRow("Maximum snapshots to keep:", m_maxSnapshotsSpin);
    
    mainLayout->addWidget(generalGroup);
    
    // Cloud sync group (placeholder for Phase 2)
    QGroupBox* cloudGroup = new QGroupBox("Cloud Sync (Coming Soon)", this);
    cloudGroup->setEnabled(false);
    QVBoxLayout* cloudLayout = new QVBoxLayout(cloudGroup);
    cloudLayout->addWidget(new QLabel("Cloud sync features will be available in a future update.", this));
    mainLayout->addWidget(cloudGroup);
    
    mainLayout->addStretch();
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::saveSettings);
    mainLayout->addWidget(buttonBox);
    
    loadSettings();
}

void SettingsDialog::loadSettings()
{
    // TODO: Load from QSettings or ProjectConfig
}

void SettingsDialog::saveSettings()
{
    // TODO: Save to QSettings or ProjectConfig
}
