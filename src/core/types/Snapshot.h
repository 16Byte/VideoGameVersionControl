#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <QString>
#include <QDateTime>

/**
 * @brief Represents a game state snapshot
 * 
 * A snapshot corresponds to a Git commit internally, but users
 * only see it as a named checkpoint they can restore to.
 */
struct Snapshot {
    QString id;              // Git commit hash
    QString description;     // User-provided or auto-generated description
    QDateTime timestamp;     // When snapshot was created
    QString author;          // Always "VGVC User" for now
    qint64 sizeBytes;       // Size of snapshot (if available, -1 if unknown)
    bool isAutomatic;        // Auto-snapshot vs manual
    
    /**
     * @brief Generate display text for UI
     * @return Formatted string: "YYYY-MM-DD HH:MM - Description"
     */
    QString displayText() const {
        return QString("%1 - %2")
            .arg(timestamp.toString("yyyy-MM-dd HH:mm"))
            .arg(description);
    }
    
    /**
     * @brief Default constructor
     */
    Snapshot()
        : sizeBytes(-1)
        , isAutomatic(false)
    {}
};

#endif // SNAPSHOT_H
