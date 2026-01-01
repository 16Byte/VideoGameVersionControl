#include "Logger.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutex>

Logger::Level Logger::s_logLevel = Logger::Level::Debug;
QString Logger::s_logFilePath;

QString Logger::levelToString(Level level)
{
    switch (level) {
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO";
        case Level::Warning: return "WARNING";
        case Level::Error:   return "ERROR";
        default:             return "UNKNOWN";
    }
}

void Logger::log(Level level, const QString& message, const QString& context)
{
    if (level < s_logLevel) {
        return;
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString levelStr = levelToString(level);
    QString contextStr = context.isEmpty() ? "" : QString(" [%1]").arg(context);
    
    QString logMessage = QString("%1 [%2]%3: %4")
        .arg(timestamp)
        .arg(levelStr)
        .arg(contextStr)
        .arg(message);
    
    // Output to console
    switch (level) {
        case Level::Debug:
            qDebug().noquote() << logMessage;
            break;
        case Level::Info:
            qInfo().noquote() << logMessage;
            break;
        case Level::Warning:
            qWarning().noquote() << logMessage;
            break;
        case Level::Error:
            qCritical().noquote() << logMessage;
            break;
    }
    
    // Write to file if configured
    if (!s_logFilePath.isEmpty()) {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        
        QFile file(s_logFilePath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << logMessage << "\n";
        }
    }
}

void Logger::debug(const QString& message, const QString& context)
{
    log(Level::Debug, message, context);
}

void Logger::info(const QString& message, const QString& context)
{
    log(Level::Info, message, context);
}

void Logger::warning(const QString& message, const QString& context)
{
    log(Level::Warning, message, context);
}

void Logger::error(const QString& message, const QString& context)
{
    log(Level::Error, message, context);
}

void Logger::setLogLevel(Level level)
{
    s_logLevel = level;
}

void Logger::setLogFile(const QString& filePath)
{
    s_logFilePath = filePath;
}
