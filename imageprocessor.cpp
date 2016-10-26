#include "imageprocessor.h"

#include <QDebug>

class SortRectByX
{
public:
    bool operator()(Rect const &a, Rect const &b) const
    {
        return a.x < b.x;
    }
};

ImageProcessor::ImageProcessor(Config *config) :
    _config(config), _debugWindow(false),
    _debugSkew(false), _debugDigits(false),
    _debugEdges(false), _detectDigits(false)
{
}

void ImageProcessor::setInput(Mat &img)
{
    _img = img;
}

const vector<Mat> &ImageProcessor::getOutput()
{
    return _digits;
}

void ImageProcessor::debugWindow(bool bval)
{
    _debugWindow = bval;

    if (_debugWindow) {
        namedWindow("ImageProcessor");
    }
}

void ImageProcessor::debugSkew(bool bval)
{
    _debugSkew = bval;
}

void ImageProcessor::debugEdges(bool bval)
{
    _debugEdges = bval;
}

void ImageProcessor::debugDigits(bool bval)
{
    _debugDigits = bval;
}

void ImageProcessor::detectDigits(bool bval)
{
    _detectDigits = bval;
}

void ImageProcessor::showImage()
{
    imshow("ImageProcessor", _img);

    waitKey(1);
}

void ImageProcessor::process()
{
    _digits.clear();

    // convert to gray
    cvtColor(_img, _imgGray, CV_BGR2GRAY);

    GaussianBlur(_imgGray, _imgGray, Size(5, 5), 2, 2);
    adaptiveThreshold(_imgGray, _imgGray, 255, 1, 1, 11, 2);

    // initial rotation to get the digits up
    rotate(_config->getRotationDegrees());

    // detect and correct remaining skew (+- 30 deg)
    float skew_deg = detectSkew();
    rotate(skew_deg);

    // find and isolate counter digits
    findCounterDigits();

    if (_debugWindow) {
        showImage();
    }
}

void ImageProcessor::rotate(double rotationDegrees)
{
    Mat M = getRotationMatrix2D(Point(_imgGray.cols / 2, _imgGray.rows / 2), rotationDegrees, 1);
    Mat img_rotated;

    warpAffine(_imgGray, img_rotated, M, _imgGray.size());
    _imgGray = img_rotated;

    if (_debugWindow) {
        warpAffine(_img, img_rotated, M, _img.size());
        _img = img_rotated;
    }
}

void ImageProcessor::drawLines(vector<Vec2f> &lines)
{
    // draw lines
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];

        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;

        Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));

        line(_img, pt1, pt2, Scalar(255, 0, 0), 1);
    }
}

void ImageProcessor::drawLines(vector<Vec4i> &lines, int xoff, int yoff)
{
    for (size_t i = 0; i < lines.size(); i++) {
        line(_img, Point(lines[i][0] + xoff, lines[i][1] + yoff),
             Point(lines[i][2] + xoff, lines[i][3] + yoff), Scalar(255, 0, 0), 1);
    }
}

float ImageProcessor::detectSkew()
{
    Mat edges = cannyEdges();

    // find lines
    std::vector<Vec2f> lines;
    HoughLines(edges, lines, 1, CV_PI / 180.f, 140);

    // filter lines by theta and compute average
    vector<Vec2f> filteredLines;
    float theta_min = 60.f * CV_PI / 180.f;
    float theta_max = 120.f * CV_PI / 180.0f;

    float theta_avr = 0.f;
    float theta_deg = 0.f;

    for (size_t i = 0; i < lines.size(); i++) {
        float theta = lines[i][1];

        if (theta >= theta_min && theta <= theta_max) {
            filteredLines.push_back(lines[i]);
            theta_avr += theta;
        }
    }

    if (filteredLines.size() > 0) {
        theta_avr /= filteredLines.size();
        theta_deg = (theta_avr / CV_PI * 180.f) - 90;

        qDebug() << "detectSkew: " << theta_deg << " deg";
    } else {
        qDebug() << "failed to detect skew";
    }

    if (_debugSkew) {
        drawLines(filteredLines);
    }

    return theta_deg;
}

Mat ImageProcessor::cannyEdges()
{
    Mat edges, destImage;

    int size = 2;

    Mat element = getStructuringElement(MORPH_RECT, Size(2 * size + 1, 2 * size + 1), Point(size, size));

    // Apply the dilation operation
    dilate(_imgGray, destImage, element);

    // detect edges
    Canny(destImage, edges, _config->getCannyThreshold1(), _config->getCannyThreshold2(), 5);

    return edges;
}

void ImageProcessor::findAlignedBoxes(vector<Rect>::const_iterator begin, vector<Rect>::const_iterator end, vector<Rect> &result)
{
    vector<Rect>::const_iterator it = begin;
    Rect start = *it;
    ++it;
    result.push_back(start);

    for (; it != end; ++it) {
        if (abs(start.y - it->y) < _config->getDigitYAlignment() && abs(start.height - it->height) < 5) {
            result.push_back(*it);
        }
    }
}

void ImageProcessor::filterContours(vector<vector<Point> > &contours, vector<Rect> &boundingBoxes, vector<vector<Point> > &filteredContours)
{
    // filter contours by bounding rect size
    for (size_t i = 0; i < contours.size(); i++) {
        Rect bounds = boundingRect(contours[i]);

        if (bounds.height > _config->getDigitMinHeight() && bounds.height < _config->getDigitMaxHeight() && bounds.width > 5 && bounds.width < bounds.height) {
            boundingBoxes.push_back(bounds);
            filteredContours.push_back(contours[i]);
        }
    }
}

void ImageProcessor::findCounterDigits()
{
    // edge image
    Mat edges = cannyEdges();

    if (_debugEdges) {
        imshow("edges", edges);
    }

    Mat img_ret = edges.clone();

    // find contours in whole image
    vector<vector<Point> > contours, filteredContours;
    vector<Rect> boundingBoxes;

    findContours(edges, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // filter contours by bounding rect size
    filterContours(contours, boundingBoxes, filteredContours);

    qDebug() << "number of filtered contours: " << filteredContours.size();

    // find bounding boxes that are aligned at y position
    vector<Rect> alignedBoundingBoxes, tmpRes;

    for (vector<Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
        tmpRes.clear();
        findAlignedBoxes(ib, boundingBoxes.end(), tmpRes);

        if (tmpRes.size() > alignedBoundingBoxes.size()) {
            alignedBoundingBoxes = tmpRes;
        }
    }

    if (_detectDigits) {
        vector<Rect> temp;

        for (vector<Rect>::const_iterator ib = boundingBoxes.begin(); ib != boundingBoxes.end(); ++ib) {
            if (std::find(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), (*ib)) == alignedBoundingBoxes.end()) {
                temp.push_back(*ib);
            }
        }

        if (temp.size() == 1) {
            alignedBoundingBoxes.push_back(temp[0]);
        } else {
            for (int i = 0; i < temp.size() - 1; ++i) {
                for (int j = 1; j < temp.size(); ++j) {
                    if (abs(temp[i].width - temp[j].width) <= 30) {
                        Rect rect;

                        if (temp[i].x > temp[j].x) {
                            rect.x = temp[j].x;
                        } else {
                            rect.x = temp[i].x;
                        }

                        if (temp[i].x > temp[j].x) {
                            rect.y = temp[j].y;
                        } else {
                            rect.y = temp[i].y;
                        }

                        if (temp[i].width > temp[j].width) {
                            rect.width = temp[i].width;
                        } else {
                            rect.width = temp[j].width;
                        }

                        rect.height = temp[j].height + temp[j].height;

                        alignedBoundingBoxes.push_back(rect);
                    }
                }
            }
        }
    }

    qDebug() << "max number of alignedBoxes: " << alignedBoundingBoxes.size();

    // sort bounding boxes from left to right
    qSort(alignedBoundingBoxes.begin(), alignedBoundingBoxes.end(), SortRectByX());

    if (_debugEdges) {
        // draw contours
        Mat cont = Mat::zeros(edges.rows, edges.cols, CV_8UC1);
        drawContours(cont, filteredContours, -1, Scalar(255));

        imshow("contours", cont);
    }

    // cut out found rectangles from edged image
    for (int i = 0; i < alignedBoundingBoxes.size(); ++i) {
        Rect roi = alignedBoundingBoxes[i];
        _digits.push_back(img_ret(roi));

        if (_debugDigits) {
            rectangle(_img, roi, Scalar(0, 0, 255), 1);
        }
    }
}
