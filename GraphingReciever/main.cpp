#include "datalogger.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataLogger w;
    w.show();

    return a.exec();
}
