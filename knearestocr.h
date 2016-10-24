/*
 * KNearestOcr.h
 *
 */

#ifndef KNEARESTOCR_H_
#define KNEARESTOCR_H_

#include <QObject>

#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "config.h"

using namespace cv;

class KNearestOcr
{
public:
    KNearestOcr(Config *config);
    virtual ~KNearestOcr();

    int learn(const Mat &img);
    int learn(const vector<Mat> &images);

    void saveTrainingData();
    bool loadTrainingData();

    char recognize(const Mat &img);
    QString recognize(const vector<Mat> &images);

private:
    Mat prepareSample(const Mat &img);
    void initModel();

private:
    Mat _samples;
    Mat _responses;

    CvKNearest *_pModel;
    Config *_config;
};

#endif /* KNEARESTOCR_H_ */
