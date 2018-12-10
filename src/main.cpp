#include "src/ui/MainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QTranslator translator;

    time_t epoch;

    cout << "Epoch is "
         << epoch
         << endl;

    // We need to complete the structure of this application so that things may start to happend!?

    if (translator.load("Turkish.qm", QLatin1String(":/translation")))
        QApplication::installTranslator(&translator);

    QApplication::setWindowIcon(QIcon(":/drawable/ic_launcher.png"));

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
