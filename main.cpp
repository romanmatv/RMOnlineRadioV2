#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("romanmatv developer");
    QCoreApplication::setOrganizationDomain("romanmatv.ru");
    QCoreApplication::setApplicationName("RM Online Radio");
    QCoreApplication::setApplicationVersion("0.2.0");

    QApplication::setOrganizationName("romanmatv developer");
    QApplication::setOrganizationDomain("romanmatv.ru");
    QApplication::setApplicationName("RM Online Radio");
    QApplication::setApplicationVersion("0.2.0");

    QTranslator Translator;
    QStringList LangPaths;
    LangPaths << QLibraryInfo::location(QLibraryInfo::TranslationsPath) << a.applicationDirPath();

    for (int i = 0; i < LangPaths.count(); i++) {
            if (Translator.load("qt_ru", LangPaths[i])) {
                QCoreApplication::installTranslator(&Translator);
                break;
            }
    }

    MainWindow w;
    w.show();

    return a.exec();
}
