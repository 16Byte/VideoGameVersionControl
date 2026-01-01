#include "PresetManager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>

PresetManager::PresetManager(QObject* parent)
    : QObject(parent)
{
    // Load built-in presets on construction
    loadBuiltInPresets();
}

Result<QList<GamePreset>, QString> PresetManager::loadBuiltInPresets()
{
    m_presets.clear();
    QList<GamePreset> presetList;
    
    // Get presets directory
    QString presetsPath = QCoreApplication::applicationDirPath() + "/resources/presets";
    QDir presetsDir(presetsPath);
    
    if (!presetsDir.exists()) {
        return Result<QList<GamePreset>, QString>::err("Presets directory not found");
    }
    
    // Load all .json files
    QStringList filters;
    filters << "*.json";
    QFileInfoList files = presetsDir.entryInfoList(filters, QDir::Files);
    
    for (const QFileInfo& fileInfo : files) {
        auto result = parsePresetFile(fileInfo.absoluteFilePath());
        if (result.isOk()) {
            GamePreset preset = result.value();
            m_presets[preset.gameId] = preset;
            presetList.append(preset);
        }
    }
    
    return Result<QList<GamePreset>, QString>::ok(presetList);
}

Result<GamePreset, QString> PresetManager::parsePresetFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return Result<GamePreset, QString>::err("Failed to open preset file");
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (!doc.isObject()) {
        return Result<GamePreset, QString>::err("Invalid JSON format");
    }
    
    QJsonObject obj = doc.object();
    GamePreset preset;
    
    preset.gameId = obj["game_id"].toString();
    preset.displayName = obj["display_name"].toString();
    preset.largeFileWarningMB = obj["large_file_warning_mb"].toInteger(5000);
    
    // Parse detection paths
    QJsonObject detection = obj["detection"].toObject();
    for (const QString& platform : detection.keys()) {
        QJsonArray paths = detection[platform].toArray();
        QStringList pathList;
        for (const QJsonValue& val : paths) {
            pathList.append(val.toString());
        }
        preset.detectionPaths[platform] = pathList;
    }
    
    // Parse tracked paths
    QJsonArray tracked = obj["tracked_paths"].toArray();
    for (const QJsonValue& val : tracked) {
        preset.trackedPaths.append(val.toString());
    }
    
    // Parse ignore patterns
    QJsonArray ignore = obj["ignore_patterns"].toArray();
    for (const QJsonValue& val : ignore) {
        preset.ignorePatterns.append(val.toString());
    }
    
    if (!preset.isValid()) {
        return Result<GamePreset, QString>::err("Preset validation failed");
    }
    
    return Result<GamePreset, QString>::ok(preset);
}

Result<GamePreset, QString> PresetManager::loadPreset(const QString& presetId)
{
    if (!m_presets.contains(presetId)) {
        return Result<GamePreset, QString>::err("Preset not found");
    }
    
    return Result<GamePreset, QString>::ok(m_presets[presetId]);
}

Result<QString, QString> PresetManager::detectGame(const QString& path)
{
    // Try to detect which game preset matches the given path
    for (auto it = m_presets.begin(); it != m_presets.end(); ++it) {
        const GamePreset& preset = it.value();
        
        // Check detection paths for current platform
#ifdef Q_OS_WIN
        QString platform = "windows";
#elif defined(Q_OS_MAC)
        QString platform = "macos";
#else
        QString platform = "linux";
#endif
        
        if (preset.detectionPaths.contains(platform)) {
            const QStringList& paths = preset.detectionPaths[platform];
            for (const QString& detectionPath : paths) {
                // Expand ~ to home directory
                QString expandedPath = detectionPath;
                if (expandedPath.startsWith("~")) {
                    expandedPath = QDir::homePath() + expandedPath.mid(1);
                }
                
                // Check if the path matches
                if (path.contains(expandedPath, Qt::CaseInsensitive)) {
                    return Result<QString, QString>::ok(preset.gameId);
                }
            }
        }
    }
    
    return Result<QString, QString>::err("Game not detected");
}

Result<void, QString> PresetManager::applyPreset(const QString& repoPath, const GamePreset& preset)
{
    // Create .gitignore file with preset's ignore patterns
    createGitignore(repoPath, preset.ignorePatterns);
    
    // TODO: Apply tracked paths configuration if needed
    
    return Result<void, QString>::ok();
}

void PresetManager::createGitignore(const QString& repoPath, const QStringList& patterns)
{
    QString gitignorePath = repoPath + "/.gitignore";
    QFile file(gitignorePath);
    
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "# Auto-generated by VideoGameVersionControl\n\n";
        
        for (const QString& pattern : patterns) {
            out << pattern << "\n";
        }
        
        file.close();
    }
}
