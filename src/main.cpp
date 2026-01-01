#include "ui/MainWindow.h"
#include "utils/Logger.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("VideoGameVersionControl");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("VGVC Team");
    QApplication::setOrganizationDomain("github.com/vgvc");
    
    // Initialize logger
    Logger::setLogLevel(Logger::Level::Debug);
    Logger::info("Application starting", "Main");
    
    // Set application style
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    Logger::info("Main window displayed", "Main");
    
    int result = app.exec();
    
    Logger::info("Application shutting down", "Main");
    return result;
}
