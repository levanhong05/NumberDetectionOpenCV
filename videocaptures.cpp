#include "videocaptures.h"

VideoCaptures::VideoCaptures(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(capture()));

    timer->start(5000);
}

void VideoCaptures::capture()
{
    cam = new VideoCapture(0);

    if (!cam->isOpened()) {
        qDebug() << "Cannot open Camera";
    } else {
        qDebug() << "Camera open.";
    }

    Mat frame;

    qDebug() << "Made frame.\n";
    cam->read(frame); // get a new frame from camera

    cam->release();

    if (frame.data) {
        emit requestDetect(frame);
    }
}

