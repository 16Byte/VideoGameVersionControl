#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QFuture>
#include "core/types/Result.h"

/**
 * @brief File system utility functions
 */
class FileUtils {
public:
    /**
     * @brief Calculate directory size recursively
     */
    static QFuture<Result<qint64, QString>> getDirectorySize(const QString& path);
    
    /**
     * @brief Copy directory recursively
     */
    static QFuture<Result<void, QString>> copyDirectory(
        const QString& source, const QString& destination);
    
    /**
     * @brief Check if path exists and is accessible
     */
    static bool isAccessible(const QString& path);
    
    /**
     * @brief Format size in bytes to human-readable string
     */
    static QString formatSize(qint64 bytes);
    
    /**
     * @brief Check if directory is a git repository
     */
    static bool isGitRepository(const QString& path);
    
private:
    static qint64 calculateDirSizeRecursive(const QString& path);
};

#endif // FILEUTILS_H
