#include "mainwindow.h"
#include <QApplication>
#include "dataloader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.showMaximized();

    EddyconCDataLoader dataLoader;
    dataLoader.load("RSN.res");

    return a.exec();
}
