#include <exception>

#include "knearestocr.h"

#include <QDebug>

KNearestOcr::KNearestOcr(Config *config) :
    _pModel(0), _config(config)
{
}

KNearestOcr::~KNearestOcr()
{
    if (_pModel) {
        delete _pModel;
    }
}

int KNearestOcr::learn(const Mat &img, int value)
{
    imshow("Lear", img);

    _responses.push_back(Mat(1, 1, CV_32F, value));
    _samples.push_back(prepareSample(img));

    return value;
}

int KNearestOcr::learn(const vector<Mat> &images, int value)
{
    int key = 0;

    for (vector<Mat>::const_iterator it = images.begin(); it < images.end(); ++it) {
        key = learn(*it, value);
    }

    return key;
}

void KNearestOcr::saveTrainingData()
{
    FileStorage fs(_config->getTrainingDataFilename().toStdString(), FileStorage::WRITE);

    fs << "samples" << _samples;
    fs << "responses" << _responses;

    fs.release();
}

bool KNearestOcr::loadTrainingData()
{
    FileStorage fs(_config->getTrainingDataFilename().toStdString(), FileStorage::READ);

    if (fs.isOpened()) {
        fs["samples"] >> _samples;
        fs["responses"] >> _responses;
        fs.release();

        initModel();
    } else {
        return false;
    }

    return true;
}

char KNearestOcr::recognize(const Mat &img)
{
    char cres = '?';

    try {
        if (!_pModel) {
            throw std::runtime_error("Model is not initialized");
        }

        Mat results, neighborResponses, dists;

        float result = _pModel->find_nearest(prepareSample(img), 2, results, neighborResponses, dists);

        if (0 == int(neighborResponses.at<float>(0, 0) - neighborResponses.at<float>(0, 1)) && dists.at<float>(0, 0) < _config->getOcrMaxDist()) {
            //cres = '0' + (int) result;
        } else {

        }

        qDebug() << "OCR detected: " << (int) result;

        cres = '0' + (int) result;
    } catch (std::exception &e) {
        qDebug() << e.what();
    }

    return cres;
}

QString KNearestOcr::recognize(const vector<Mat> &images)
{
    QString result;

    for (vector<Mat>::const_iterator it = images.begin(); it != images.end(); ++it) {
        result += QString(recognize(*it));
    }

    return result;
}

Mat KNearestOcr::prepareSample(const Mat &img)
{
    Mat roi, sample;
    resize(img, roi, Size(10, 10));

    roi.reshape(1, 1).convertTo(sample, CV_32F);

    return sample;
}

void KNearestOcr::initModel()
{
    _pModel = new CvKNearest(_samples, _responses);
}
