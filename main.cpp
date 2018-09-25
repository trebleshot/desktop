#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QResource>
#include <QTranslator>
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);
    QTranslator translator;
    QResource iconRes(":/drawable/ic_launcher.png", QLocale::system());

    // We need to complete the structure of this application so that things may start to happend!?

    if (translator.load("Turkish.qm", QLatin1String(":/translation")))
        application.installTranslator(&translator);

    MainWindow mainWindow;

    application.setWindowIcon(QIcon(":/drawable/ic_launcher.png"));

    mainWindow.show();

    return application.exec();
}
