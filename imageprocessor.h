#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "config.h"

using namespace cv;
using namespace std;

#define min(a, b) (a > b ? b :a)

class ImageProcessor
{
public:
    ImageProcessor(Config *config);

    void setOrientation(int rotationDegrees);

    void setInput(Mat &img);
    void process();
    const vector<Mat> &getOutput();

    void debugWindow(bool bval = true);
    void debugSkew(bool bval = true);
    void debugEdges(bool bval = true);
    void debugDigits(bool bval = true);
    void detectDigits(bool bval = true);

    void showImage();

private:
    void rotate(double rotationDegrees);
    void findCounterDigits();

    void findAlignedBoxes(vector<Rect>::const_iterator begin,
                          vector<Rect>::const_iterator end, vector<Rect> &result);
    float detectSkew();

    void drawLines(vector<Vec2f> &lines);
    void drawLines(vector<Vec4i> &lines, int xoff = 0, int yoff = 0);
    Mat cannyEdges();

    void filterContours(vector<vector<Point> > &contours, vector<Rect> &boundingBoxes,
                        vector<vector<Point> > &filteredContours);

private:
    Mat _img;
    Mat _imgGray;
    vector<Mat> _digits;

    Config *_config;

    bool _debugWindow;
    bool _debugSkew;
    bool _debugEdges;
    bool _debugDigits;

    bool _detectDigits;
};

#endif // IMAGEPROCESSOR_H
