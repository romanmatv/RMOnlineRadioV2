#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("romanmatv developer");
    QCoreApplication::setOrganizationDomain("romanmatv.ru");
    QCoreApplication::setApplicationName("RM Online Radio");
    QCoreApplication::setApplicationVersion("0.0.1");

    QApplication::setOrganizationName("romanmatv developer");
    QApplication::setOrganizationDomain("romanmatv.ru");
    QApplication::setApplicationName("RM Online Radio");
    QApplication::setApplicationVersion("0.0.1");

    //QTextCodec *codec = QTextCodec::codecForName("UTF8");
    //QTextCodec::setCodecForLocale(codec);

    MainWindow w;
    w.show();

    return a.exec();
}
