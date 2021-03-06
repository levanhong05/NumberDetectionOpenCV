#include <opencv2/highgui/highgui.hpp>

#include "config.h"

using namespace cv;

Config::Config() :
    _rotationDegrees(0), _ocrMaxDist(5e5), _digitMinHeight(20), _digitMaxHeight(100),
    _digitYAlignment(10), _cannyThreshold1(100), _cannyThreshold2(200),
    _trainingDataFilename("training.yml")
{
}

void Config::saveConfig()
{
    FileStorage fs("config.yml", FileStorage::WRITE);
    fs << "rotationDegrees" << _rotationDegrees;
    fs << "cannyThreshold1" << _cannyThreshold1;
    fs << "cannyThreshold2" << _cannyThreshold2;
    fs << "digitMinHeight" << _digitMinHeight;
    fs << "digitMaxHeight" << _digitMaxHeight;
    fs << "digitYAlignment" << _digitYAlignment;
    fs << "ocrMaxDist" << _ocrMaxDist;
    fs << "trainingDataFilename" << _trainingDataFilename.toStdString();
    fs.release();
}

void Config::loadConfig()
{
    FileStorage fs("config.yml", FileStorage::READ);

    if (fs.isOpened()) {
        fs["rotationDegrees"] >> _rotationDegrees;
        fs["cannyThreshold1"] >> _cannyThreshold1;
        fs["cannyThreshold2"] >> _cannyThreshold2;
        fs["digitMinHeight"] >> _digitMinHeight;
        fs["digitMaxHeight"] >> _digitMaxHeight;
        fs["digitYAlignment"] >> _digitYAlignment;
        fs["ocrMaxDist"] >> _ocrMaxDist;
        fs["trainingDataFilename"] >> _trainingDataFilename.toStdString();
        fs.release();
    } else {
        // no config file - create an initial one with default values
        saveConfig();
    }
}
