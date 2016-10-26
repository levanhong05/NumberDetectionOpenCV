#include <QCoreApplication>
#include <QObject>

#include "numberdetection.h"

#include "videocaptures.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //VideoCaptures *capture = new VideoCaptures();

    NumberDetection *detector = new NumberDetection();

    //QObject::connect(capture, SIGNAL(requestDetect(Mat)), detector, SLOT(analyseImage(Mat)));
    detector->detectNumber();

    return a.exec();
}
