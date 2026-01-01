#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDebug>

/**
 * @brief Simple logging utility
 * 
 * Provides structured logging for the application.
 */
class Logger {
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };
    
    static void log(Level level, const QString& message, const QString& context = QString());
    
    static void debug(const QString& message, const QString& context = QString());
    static void info(const QString& message, const QString& context = QString());
    static void warning(const QString& message, const QString& context = QString());
    static void error(const QString& message, const QString& context = QString());
    
    static void setLogLevel(Level level);
    static void setLogFile(const QString& filePath);
    
private:
    static Level s_logLevel;
    static QString s_logFilePath;
    
    static QString levelToString(Level level);
};

#endif // LOGGER_H
