#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <QString>
#include <QDateTime>

/**
 * @brief Per-project configuration
 * 
 * Stores settings specific to each game project.
 */
class ProjectConfig {
public:
    ProjectConfig();
    
    // Project identity
    QString projectPath;
    QString gameId;
    QString gameName;
    
    // Settings
    bool autoSnapshotOnClose;
    int maxSnapshots;  // -1 for unlimited
    qint64 maxSizeBytes;  // -1 for unlimited
    
    // Cloud sync (Phase 2)
    bool cloudSyncEnabled;
    QString remoteUrl;
    QDateTime lastSyncTime;
    
    // Load/save configuration
    bool load(const QString& configPath);
    bool save(const QString& configPath) const;
    
private:
    void setDefaults();
};

#endif // PROJECTCONFIG_H
