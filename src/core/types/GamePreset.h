#ifndef GAMEPRESET_H
#define GAMEPRESET_H

#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief Game-specific configuration preset
 * 
 * Defines which files to track, which to ignore, and how to detect
 * the game installation on different platforms.
 */
struct GamePreset {
    QString gameId;                              // Unique identifier (e.g., "skyrim_se")
    QString displayName;                         // User-friendly name
    QMap<QString, QStringList> detectionPaths;   // platform -> list of common paths
    QStringList trackedPaths;                    // Paths/patterns to track in git
    QStringList ignorePatterns;                  // Patterns for .gitignore
    qint64 largeFileWarningMB;                  // Warn if total size exceeds this
    
    /**
     * @brief Default constructor
     */
    GamePreset()
        : largeFileWarningMB(5000)  // Default 5GB warning
    {}
    
    /**
     * @brief Check if this preset is valid
     */
    bool isValid() const {
        return !gameId.isEmpty() && !displayName.isEmpty();
    }
};

#endif // GAMEPRESET_H
