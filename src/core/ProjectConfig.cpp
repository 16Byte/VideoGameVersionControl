#include "ProjectConfig.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

ProjectConfig::ProjectConfig()
{
    setDefaults();
}

void ProjectConfig::setDefaults()
{
    autoSnapshotOnClose = true;
    maxSnapshots = 50;
    maxSizeBytes = 5 * 1024 * 1024 * 1024LL;  // 5GB
    cloudSyncEnabled = false;
}

bool ProjectConfig::load(const QString& configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        return false;
    }
    
    QJsonObject obj = doc.object();
    
    projectPath = obj["project_path"].toString();
    gameId = obj["game_id"].toString();
    gameName = obj["game_name"].toString();
    autoSnapshotOnClose = obj["auto_snapshot_on_close"].toBool(true);
    maxSnapshots = obj["max_snapshots"].toInt(50);
    maxSizeBytes = obj["max_size_bytes"].toInteger(5LL * 1024 * 1024 * 1024);
    cloudSyncEnabled = obj["cloud_sync_enabled"].toBool(false);
    remoteUrl = obj["remote_url"].toString();
    
    QString lastSyncStr = obj["last_sync_time"].toString();
    if (!lastSyncStr.isEmpty()) {
        lastSyncTime = QDateTime::fromString(lastSyncStr, Qt::ISODate);
    }
    
    return true;
}

bool ProjectConfig::save(const QString& configPath) const
{
    QJsonObject obj;
    obj["project_path"] = projectPath;
    obj["game_id"] = gameId;
    obj["game_name"] = gameName;
    obj["auto_snapshot_on_close"] = autoSnapshotOnClose;
    obj["max_snapshots"] = maxSnapshots;
    obj["max_size_bytes"] = static_cast<qint64>(maxSizeBytes);
    obj["cloud_sync_enabled"] = cloudSyncEnabled;
    obj["remote_url"] = remoteUrl;
    
    if (lastSyncTime.isValid()) {
        obj["last_sync_time"] = lastSyncTime.toString(Qt::ISODate);
    }
    
    QJsonDocument doc(obj);
    
    QFile file(configPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    return true;
}
