#ifndef MFRECONSTRUCT_H
#define MFRECONSTRUCT_H

#include <QObject>

#include <opencv/cv.h>

#include "virtualcamera.h"
#include "pointcloudimage.h"
#include "stereorect.h"

class MFReconstruct : public QObject
{
    Q_OBJECT
public:
    explicit MFReconstruct(QObject *parent = 0);
    ~MFReconstruct();

    bool getParameters(int scansn,  int camw, int camh, int blackt, bool havecolor, QString savePath);
    bool runReconstruction();

    PointCloudImage *points3DProjView;

private:
    int scanSN;//表示当前重建的扫描数据序列号，也是输出模型的序列号

    QString savePath_;//same as projectPath
    QString *calibFolder;
    QString *scanFolder;
    QString *imgPrefix;
    QString imgSuffix;

    int numberOfImgs;
    int numOfColBits;

    int blackThreshold;

    cv::Mat mask;//matrix with vals 0 and 1 , CV_8U , uchar
    cv::Mat decRows;
    cv::Mat decCols;
    cv::vector<cv::Mat> camImgs;

    cv::vector<double> **camsPixels;
    cv::vector<double> *camPixels;

    bool pathSet;
    bool haveColor;
    int cameraWidth;
    int cameraHeight;

//    VirtualCamera *camera;//general functions use this instead of camera1 or camera2
//    VirtualCamera	*cameras;
    stereoRect *sr;

//    bool loadCameras();
    bool loadCamImgs(QString folder, QString prefix, QString suffix);
    void unloadCamImgs();
    void computeShadows();
    void decodePatterns();
    void getPhase(int row, int col, double &phase);
    void triangulation(cv::vector<double> *cam1Pixels, cv::vector<double> *cam2Pixels);

signals:

public slots:

};

#endif // MFRECONSTRUCT_H
