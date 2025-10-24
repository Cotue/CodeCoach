#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <QApplication>
#include "Interfaz/src/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

