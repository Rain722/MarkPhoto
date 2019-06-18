#include "include/mainwindow.h"
#include <QApplication>
#include <windows.h>
#include <iostream>
#include <string>
#include <QFileDialog>

int main(int argc, char *argv[])
{

    //dd
    QApplication a(argc, argv); //doc
    MainWindow w;
    w.show();

    return a.exec();
}
