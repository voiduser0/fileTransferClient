#include "FileTransferClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileTransferClient w;
    w.show();
    return a.exec();
}
