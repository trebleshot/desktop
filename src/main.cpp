#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <iostream>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;

    mainWindow.show();

    return a.exec();
}
