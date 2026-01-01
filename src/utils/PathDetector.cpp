#include "PathDetector.h"
#include <QDir>
#include <QStandardPaths>
#include <QFile>

QString PathDetector::getCurrentPlatform()
{
#ifdef Q_OS_WIN
    return "windows";
#elif defined(Q_OS_MAC)
    return "macos";
#else
    return "linux";
#endif
}

QStringList PathDetector::getSteamLibraryPaths()
{
    QStringList paths;
    QString platform = getCurrentPlatform();
    
#ifdef Q_OS_WIN
    QString steamPath = "C:/Program Files (x86)/Steam";
    if (QDir(steamPath).exists()) {
        paths << steamPath + "/steamapps/common";
    }
#elif defined(Q_OS_MAC)
    QString steamPath = QDir::homePath() + "/Library/Application Support/Steam";
    if (QDir(steamPath).exists()) {
        paths << steamPath + "/steamapps/common";
    }
#else // Linux
    QString steamPath = QDir::homePath() + "/.steam/steam";
    if (QDir(steamPath).exists()) {
        paths << steamPath + "/steamapps/common";
    }
    
    // Also check Flatpak Steam
    QString flatpakSteam = QDir::homePath() + "/.var/app/com.valvesoftware.Steam/.steam/steam";
    if (QDir(flatpakSteam).exists()) {
        paths << flatpakSteam + "/steamapps/common";
    }
#endif
    
    return paths;
}

QStringList PathDetector::getCommonGamePaths()
{
    QStringList paths;
    
#ifdef Q_OS_WIN
    paths << "C:/Program Files"
          << "C:/Program Files (x86)"
          << QDir::homePath() + "/Documents/My Games";
#elif defined(Q_OS_MAC)
    paths << "/Applications"
          << QDir::homePath() + "/Library/Application Support"
          << QDir::homePath() + "/Documents";
#else // Linux
    paths << QDir::homePath() + "/.local/share"
          << QDir::homePath() + "/Games"
          << "/opt";
#endif
    
    // Add Steam paths
    paths << getSteamLibraryPaths();
    
    return paths;
}

QString PathDetector::expandPath(const QString& path)
{
    QString expanded = path;
    
    // Expand ~ to home directory
    if (expanded.startsWith("~")) {
        expanded = QDir::homePath() + expanded.mid(1);
    }
    
    // TODO: Expand environment variables
    
    return expanded;
}

bool PathDetector::looksLikeGameDirectory(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return false;
    }
    
    // Look for common game file indicators
    QStringList indicators = {
        "*.exe",  // Windows executables
        "*.app",  // macOS applications
        "*.so",   // Linux shared libraries
        "Data",   // Common data folder
        "Saves",  // Save files
        "*.ini",  // Config files
        "*.pak",  // Package files
        "*.unity3d"  // Unity games
    };
    
    for (const QString& pattern : indicators) {
        QStringList matches = dir.entryList(QStringList() << pattern);
        if (!matches.isEmpty()) {
            return true;
        }
    }
    
    return false;
}
