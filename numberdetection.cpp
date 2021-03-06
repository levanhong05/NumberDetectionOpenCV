#include "numberdetection.h"

#include <QDebug>

#include <iostream>
#include <QApplication>
#include <QDir>

#include "config.h"
#include "imageprocessor.h"
#include "knearestocr.h"
#include "plausi.h"

using namespace std;

NumberDetection::NumberDetection(QObject *parent) :
    QObject(parent)
{
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

    for (int i = 0; i < _classes; i++) {
        QDir trainDir(_pathImages + "/training/" + QString::number(i));
        trainDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);

        QFileInfoList samples = trainDir.entryInfoList();

        foreach (QFileInfo info, samples) {
            fileName = info.absoluteFilePath();

            img = imread(fileName.toStdString(), 1);

            if (img.data) {
                proc->setInput(img);
                proc->process();
                key = ocr->learn(proc->getOutput(), i);
            }
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
    proc->debugEdges();

    Plausi *plausi = new Plausi();

    KNearestOcr *ocr = new KNearestOcr(config);

    if (!ocr->loadTrainingData()) {
        qDebug() << "Failed to load OCR training data\n";
        return;
    }

    qDebug() << "OCR training data loaded.\n";

    Mat image = imread(QString(_pathImages + "/training/test.png").toStdString(), 1);

    proc->detectDigits();

    if (!image.data) {
        qDebug() << "File test not found";
        exit(0);
    }

    proc->setInput(image);
    proc->process();
    QString result = ocr->recognize(proc->getOutput());

    if (result.length() <= 3) {
        while (result.length() < 3) {
            result.append("0");
        }

        result.prepend("0.");
    } else if (result.length() > 6) {
        qDebug() << "Wrong detect";
    } else {
        QString decimal = result.right(3);
        QString value = result.mid(0, result.length() - 3);

        result = value + "." + decimal;
    }

    qDebug() << "Number decteted: " << result;

    if (plausi->check(result)) {
        qDebug() << "  " << plausi->getCheckedValue();
    }

    waitKey(0);
}

void NumberDetection::analyseImage(Mat image)
{
    Config *config = new Config();
    config->loadConfig();

    ImageProcessor *proc = new ImageProcessor(config);
    proc->debugWindow();
    proc->debugDigits();
    proc->debugEdges();

    Plausi *plausi = new Plausi();

    KNearestOcr *ocr = new KNearestOcr(config);

    if (!ocr->loadTrainingData()) {
        qDebug() << "Failed to load OCR training data\n";
        return;
    }

    qDebug() << "OCR training data loaded.\n";

    proc->detectDigits();

    //resizing
    resize(image, image, Size(300,500));

    if (!image.data) {
        qDebug() << "File test not found";
        exit(0);
    }

    proc->setInput(image);
    proc->process();
    QString result = ocr->recognize(proc->getOutput());

    qDebug() << "Number decteted: " << result;

    if (plausi->check(result)) {
        qDebug() << "  " << plausi->getCheckedValue();
    }

    waitKey(0);
}
