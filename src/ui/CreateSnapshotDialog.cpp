#include "CreateSnapshotDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

CreateSnapshotDialog::CreateSnapshotDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Create Snapshot");
    resize(400, 250);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // Description
    layout->addWidget(new QLabel("Description (optional):", this));
    m_descriptionEdit = new QLineEdit(this);
    m_descriptionEdit->setPlaceholderText("e.g., Before installing new mods");
    layout->addWidget(m_descriptionEdit);
    
    // Notes
    layout->addWidget(new QLabel("Additional notes:", this));
    m_notesEdit = new QTextEdit(this);
    m_notesEdit->setPlaceholderText("Optional detailed notes about this snapshot...");
    m_notesEdit->setMaximumHeight(100);
    layout->addWidget(m_notesEdit);
    
    // Info label
    QLabel* infoLabel = new QLabel(
        "If no description is provided, an automatic timestamp will be used.", this);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: gray; font-size: 10px;");
    layout->addWidget(infoLabel);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

QString CreateSnapshotDialog::getDescription() const
{
    QString description = m_descriptionEdit->text().trimmed();
    QString notes = m_notesEdit->toPlainText().trimmed();
    
    if (!notes.isEmpty()) {
        description += QString("\n\n%1").arg(notes);
    }
    
    return description;
}
