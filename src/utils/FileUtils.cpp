#include "FileUtils.h"
#include <QDir>
#include <QFileInfo>
#include <QtConcurrent>

QFuture<Result<qint64, QString>> FileUtils::getDirectorySize(const QString& path)
{
    return QtConcurrent::run([path]() -> Result<qint64, QString> {
        if (!QDir(path).exists()) {
            return Result<qint64, QString>::err("Directory does not exist");
        }
        
        qint64 size = calculateDirSizeRecursive(path);
        return Result<qint64, QString>::ok(size);
    });
}

qint64 FileUtils::calculateDirSizeRecursive(const QString& path)
{
    qint64 totalSize = 0;
    QDir dir(path);
    
    QFileInfoList entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    
    for (const QFileInfo& entry : entries) {
        if (entry.isFile()) {
            totalSize += entry.size();
        } else if (entry.isDir()) {
            totalSize += calculateDirSizeRecursive(entry.absoluteFilePath());
        }
    }
    
    return totalSize;
}

QFuture<Result<void, QString>> FileUtils::copyDirectory(
    const QString& source, const QString& destination)
{
    return QtConcurrent::run([source, destination]() -> Result<void, QString> {
        QDir sourceDir(source);
        if (!sourceDir.exists()) {
            return Result<void, QString>::err("Source directory does not exist");
        }
        
        QDir destDir(destination);
        if (!destDir.exists()) {
            if (!destDir.mkpath(".")) {
                return Result<void, QString>::err("Failed to create destination directory");
            }
        }
        
        QFileInfoList entries = sourceDir.entryInfoList(
            QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        
        for (const QFileInfo& entry : entries) {
            QString destPath = destination + "/" + entry.fileName();
            
            if (entry.isFile()) {
                if (!QFile::copy(entry.absoluteFilePath(), destPath)) {
                    return Result<void, QString>::err(
                        QString("Failed to copy file: %1").arg(entry.fileName()));
                }
            } else if (entry.isDir()) {
                auto result = copyDirectory(entry.absoluteFilePath(), destPath);
                result.waitForFinished();
                if (result.result().isErr()) {
                    return result.result();
                }
            }
        }
        
        return Result<void, QString>::ok();
    });
}

bool FileUtils::isAccessible(const QString& path)
{
    QFileInfo info(path);
    return info.exists() && info.isReadable();
}

QString FileUtils::formatSize(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;
    
    if (bytes >= GB) {
        return QString("%1 GB").arg(bytes / static_cast<double>(GB), 0, 'f', 2);
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(bytes / static_cast<double>(MB), 0, 'f', 2);
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(bytes / static_cast<double>(KB), 0, 'f', 2);
    } else {
        return QString("%1 bytes").arg(bytes);
    }
}

bool FileUtils::isGitRepository(const QString& path)
{
    QString gitDir = path + "/.git";
    QFileInfo gitInfo(gitDir);
    return gitInfo.exists() && gitInfo.isDir();
}
