#ifndef DOTMATCH_H
#define DOTMATCH_H

//调试用宏定义
//#define DEBUG
//#define TEST_SURF
//宏定义在本文件及引用的头文件中只需定义一次

// Qt
#include <QObject>

// OpenCV
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/flann/flann.hpp>

// STL
#include <math.h>

// SLR
#include "reconstruct.h"
#include "virtualcamera.h"
#include "utilities.h"
#include "blobdetector.h"
#include "manualmatch.h"
#include "stereorect.h"

// MRPT
#include <mrpt/scanmatching.h>

using namespace cv;
using namespace std;

class DotMatch : public QObject
{
    Q_OBJECT
public:

    DotMatch(QObject *parent = 0, QString projectPath = NULL, bool useManual = true, bool useepi = true);
    ~DotMatch();

    ManualMatch *mm;

    vector<vector<float>> findDot(Mat &image, bool isleft);
    bool matchDot(Mat &limage, Mat &rimage);
    void setUpManual(Mat LImage, Mat RImage);//初始化手工标记窗口
    void activeManual();//启动手工标记窗口
    void finishMatch();
    int OSTU_Region(cv::Mat &image);

    vector<vector<float>> dotInOrder;

    //表示新点与原有点对应关系，如果该点实际为原有点
    //x值为该点在dotFeature中的序号
    //y值为该点在dotPosition（even或odd）中的序号
    cv::vector<Point2i> correspondPointEven;
    cv::vector<Point2i> correspondPointOdd;

    //标记并显示标志点所需数据，int值有6个，依次为
    //左点x，y，右点x，y，是否为已知点(0，1，0代表未知)
    //如果已知，对应的唯一编号
    vector<vector<int>> dotForMark;

    int bwThreshold;
    int blocksize;//二值化的一个参数，由用户给定

    ///-------初始化所需数据--------/
    bool firstFind;
    int scanSN;
    int resetSN;//若在主窗口启用了resetfind功能，则将该值置换为将要进行的扫描序号

    static bool isBelongTo(size_t e, vector<int> C);

private:
    QString path;
    bool useManualMatch;
    bool useEpi;//表示是否采用极线校正，若采用，计算标记点坐标采用Q矩阵法
    stereoRect *sr;

    bool triangleCalculate();
    cv::vector<cv::vector<float> > calFeature(cv::vector<Point3f> dotP);
    bool dotClassify(cv::vector<cv::vector<float> > featureTemp);
    bool FTTM(cv::vector<Point2i> &correspondPoint, cv::vector<Point3f> dotPositionCurrent, cv::vector<Point3f> dotPositionFormer);
    void updateDot(cv::vector<Point2i> &correspondPoint, cv::vector<Point3f> &dotPositionCurrent, cv::vector<Point3f> dotPositionFormer);
    vector<int> calNeighbor(vector<vector<float> > input, int num);
    bool checkNeighbor(vector<int> referance, vector<int> needcheck);
    void calMatrix();
    //void hornTransform(double &data[], cv::vector<Point3f> target, cv::vector<Point3f> move);
    void markPoint();

    vector<Point2f> subPixel(Mat img, vector<vector<float>> vec);
    Reconstruct *rc;
    BlobDetector *bd;

    Mat fundMat;
    Mat Homo1;
    Mat Homo2;

    vector<int> dotRemove;//不能成功三角计算的点需要去除，该向量存储待去除点在当次扫描中的序号

    Mat outR;//保存后一次到前一次扫描的旋转变换阵
    Mat outT;//保存相后一次到前一次扫描的平移变换阵
    Mat outRInv;//保存前一次到后一次扫描的旋转变换阵
    Mat outTInv;//保存前一次到后一次扫描的旋转变换阵
    Mat matRotation;//保存广义旋转矩阵
    Mat matTransform;//保存广义平移向量
    cv::vector<Point3f> dotPositionEven;//偶数次扫描所得点的绝对坐标
    cv::vector<Point3f> dotPositionOdd;//奇数次扫描所得点的绝对坐标
    cv::vector<cv::vector<float>> dotFeature;
    vector<vector<int>> neighborFeature;

private slots:
    void onfinishmanual();

signals:
    void receivedmanualmatch();//由槽函数onfinishmanual()发出，通知MainWindow执行finishmanualmatch()槽，在该槽中又调用了finishMatch

};

class Triangle{
public:
    Triangle(int Vertex_0, int Vertex_1, int Vertex_2, float distance_12, float distance_02, float distance_01);

    static bool copmareTriangle(Triangle tri_known, Triangle tri_unknown, vector<Point2i> &corr, float &error);//比较两三角形是否相等
    static float calDistance(Point3f point_1,Point3f point_2);

    int ver_0;
    int ver_1;
    int ver_2;
    float dis_0;
    float dis_1;
    float dis_2;
};

template <typename vecType>
void eraseElement(cv::vector<vecType> &tgt, cv::vector<int> erase)
{
    cv::vector<vecType> out;
    for (size_t i=0;i<tgt.size();i++){
        if(!DotMatch::isBelongTo(i,erase)){
            out.push_back(tgt[i]);
        }
    }
    tgt.clear();
    for (size_t j=0;j<out.size();j++){
        tgt.push_back(out[j]);
    }
}


#endif // DOTMATCH_H
