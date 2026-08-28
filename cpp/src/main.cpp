#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("Windows Spotlight");

    MainWindow window;

    window.show();
    window.raise();
    window.activateWindow();

    return app.exec();
}