#include <QCoreApplication>

#include "numberdetection.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    NumberDetection *detector = new NumberDetection();
    detector->detectNumber();

    return a.exec();
}
