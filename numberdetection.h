#ifndef NUMBERDETECTION_H
#define NUMBERDETECTION_H

#include <QObject>

#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

class NumberDetection : public QObject
{
    Q_OBJECT
public:
    explicit NumberDetection(QObject *parent = 0);

    void detectNumber();

    void trainingFromImages();
    void analyseImage();

private:
    int _trainSamples;
    int _classes;

    QString _pathImages;

};

#endif // NUMBERDETECTION_H
