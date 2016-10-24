#include "numberdetection.h"

#include <QDebug>

#include <iostream>
#include <QApplication>

#include "config.h"
#include "imageprocessor.h"
#include "knearestocr.h"
#include "plausi.h"

using namespace std;

NumberDetection::NumberDetection(QObject *parent) :
    QObject(parent)
{
    _trainSamples = 3;
    _classes = 10;

    _pathImages = QApplication::applicationDirPath();
}

void NumberDetection::detectNumber()
{
    trainingFromImages();
    analyseImage();

    exit(0);
}

void NumberDetection::trainingFromImages()
{
    Config *config = new Config();
    config->loadConfig();

    ImageProcessor *proc = new ImageProcessor(config);
    proc->debugWindow();

    KNearestOcr *ocr = new KNearestOcr(config);
    ocr->loadTrainingData();

    int key = 0;

    Mat img;
    QString fileName;
    for (int idx = 0; idx < _trainSamples; idx++) {
        for (int i = 0; i < _classes; i++) {
            fileName = _pathImages + "/training/" + QString::number(idx + 1) + "/" + QString::number(i) + ".png";

            img = imread(fileName.toStdString(), 1);

            if (!img.data) {
                qDebug() << "File " << fileName << " not found";
                exit(1);
            }

            proc->setInput(img);
            proc->process();
            key = ocr->learn(proc->getOutput());
        }
    }

    ocr->saveTrainingData();
}

void NumberDetection::analyseImage()
{
    Config *config = new Config();
    config->loadConfig();

    ImageProcessor *proc = new ImageProcessor(config);
    proc->debugWindow();
    proc->debugDigits();
    proc->debugSkew();
    proc->debugEdges();

    Plausi *plausi = new Plausi();

    KNearestOcr *ocr = new KNearestOcr(config);

    if (!ocr->loadTrainingData()) {
        qDebug() << "Failed to load OCR training data\n";
        return;
    }

    qDebug() << "OCR training data loaded.\n";

    Mat image = imread(QString(_pathImages + "/training/test4.png").toStdString(), 1);

    proc->setInput(image);
    proc->process();
    QString result = ocr->recognize(proc->getOutput());

    qDebug() << "Number decteted: " << result;

    if (plausi->check(result)) {
        qDebug() << "  " << plausi->getCheckedValue();
    } else {
        qDebug() << "  -------";
    }

    cv::waitKey(0);
}
