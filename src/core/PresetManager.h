#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QObject>
#include <QMap>
#include "types/Result.h"
#include "types/GamePreset.h"

/**
 * @brief Manages game presets and .gitignore configuration
 * 
 * Loads built-in game presets and applies them to repositories.
 */
class PresetManager : public QObject {
    Q_OBJECT
    
public:
    explicit PresetManager(QObject* parent = nullptr);
    ~PresetManager() override = default;
    
    Result<QList<GamePreset>, QString> loadBuiltInPresets();
    Result<GamePreset, QString> loadPreset(const QString& presetId);
    Result<QString, QString> detectGame(const QString& path);
    Result<void, QString> applyPreset(const QString& repoPath, const GamePreset& preset);
    
private:
    QMap<QString, GamePreset> m_presets;
    
    Result<GamePreset, QString> parsePresetFile(const QString& filePath);
    void createGitignore(const QString& repoPath, const QStringList& patterns);
};

#endif // PRESETMANAGER_H
