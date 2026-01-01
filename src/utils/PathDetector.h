#ifndef PATHDETECTOR_H
#define PATHDETECTOR_H

#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief Detects game installation paths on different platforms
 */
class PathDetector {
public:
    /**
     * @brief Find Steam library folders
     */
    static QStringList getSteamLibraryPaths();
    
    /**
     * @brief Find common game installation directories
     */
    static QStringList getCommonGamePaths();
    
    /**
     * @brief Expand path with environment variables and ~ 
     */
    static QString expandPath(const QString& path);
    
    /**
     * @brief Check if a path looks like a game directory
     */
    static bool looksLikeGameDirectory(const QString& path);
    
private:
    static QString getCurrentPlatform();
};

#endif // PATHDETECTOR_H
