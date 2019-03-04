#include <src/build.h>
#include <src/config/Config.h>
#include <src/ui/MainWindow.h>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    initAppEnvironment();

    //todo: Implement language support
    /**
    QTranslator translator;

    if (translator.load("Turkish.qm", QLatin1String(":/translation")))
        QApplication::installTranslator(&translator);
     **/

    QApplication::setWindowIcon(QIcon(":/logo/launcher"));
    QApplication::setApplicationName(TREBLESHOT_NAME);
    QApplication::setApplicationVersion(TREBLESHOT_VERSION_NAME);
    QApplication::setApplicationDisplayName(TREBLESHOT_NAME);

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}