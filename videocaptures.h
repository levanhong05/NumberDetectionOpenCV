#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>

#include <QObject>

#include <QDebug>
#include <QTimer>
#include <QDateTime>

using namespace cv;

class VideoCaptures : public QObject
{
    Q_OBJECT
public:
    explicit VideoCaptures(QObject *parent = 0);

public slots:
    void capture();

signals:
    void requestDetect(Mat);

private:
    VideoCapture *cam;

    QTimer *timer;
};

#endif // VIDEOCAPTURE_H
