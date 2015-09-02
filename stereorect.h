#ifndef STEREORECT_H
#define STEREORECT_H

#include <QObject>

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/contrib/contrib.hpp>

#include "utilities.h"

//#define TEST//测试二值化

class stereoRect : public QObject
{
    Q_OBJECT
public:
    stereoRect(QObject *parent = 0, QString projectPath = NULL, cv::Size size = cv::Size(0,0));
    ~stereoRect();

    void doStereoRectify(cv::Mat &img, bool isleft);
    void getParameters();
    void calParameters();
    cv::Mat R1, P1, R2, P2, Q;//输出参数

private:
    QString ppath;
    cv::Size img_size;
    cv::Mat M1, D1, M2, D2, R, T;//输入参数
    cv::Mat map11, map12, map21, map22;//中间参数
    void loadMatrix(cv::Mat &matrix, int rows, int cols, QString file);
};
#endif // STEREORECT_H
