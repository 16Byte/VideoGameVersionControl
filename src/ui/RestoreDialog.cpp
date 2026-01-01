#include "RestoreDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

RestoreDialog::RestoreDialog(const Snapshot& snapshot, QWidget* parent)
    : QDialog(parent)
    , m_snapshot(snapshot)
{
    setWindowTitle("Restore Snapshot");
    resize(500, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // Snapshot info
    layout->addWidget(new QLabel("Snapshot Information:", this));
    
    QLabel* infoLabel = new QLabel(this);
    infoLabel->setText(QString(
        "Description: %1\n"
        "Created: %2\n"
        "ID: %3"
    ).arg(snapshot.description)
     .arg(snapshot.timestamp.toString("yyyy-MM-dd HH:mm:ss"))
     .arg(snapshot.id.left(10)));
    layout->addWidget(infoLabel);
    
    layout->addSpacing(20);
    
    // Warning
    QLabel* warningLabel = new QLabel(
        "⚠ Warning: This will revert your game files to the state of this snapshot.\n"
        "A safety backup will be created automatically.", this);
    warningLabel->setWordWrap(true);
    warningLabel->setStyleSheet("color: orange; font-weight: bold;");
    layout->addWidget(warningLabel);
    
    layout->addStretch();
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}
