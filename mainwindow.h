#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Qt
#include <QMainWindow>
#include <QtGui>
#include <QLabel> 
#include <QImage>
#include <QTimer>

//openCV
#include <opencv/highgui.h>
#include <opencv/cv.h>

//SLR
#include "blobdetector.h"
#include "cameracalibration.h"
#include "dotmatch.h"
#include "glwidget.h"

#include "projector.h"
#include "reconstruct.h"
#include "mfreconstruct.h"
#include "meshcreator.h"

#include "set.h"
#include "focusassistant.h"

#include "graycodes.h"
#include "multifrequency.h"

#include "dahengcamera.h"

#include "imageviewer.h"

#include "stereorect.h"

//#define DEBUG//用来观察标记点圆心定位精度

#define WM_SNAP_CHANGE		(WM_USER + 100)

#define CALIBIMGNUM 14

#define PATHCALIB 0
#define PATHSCAN 1
#define PATHRECON 2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    FocusAssistant *fa;
    Set *setDialog;
    DotMatch *dm;
    GLWidget *displayModel;

    QString projectPath;
    QString projChildPath;

    int screenWidth;//主屏幕几何尺寸
    int screenHeight;
    int projectorWidth;//投影屏幕几何尺寸
    int projectorHeight;
    int scanWidth;//扫描区域尺寸
    int scanHeight;
    int cameraWidth;//相机分辨率
    int cameraHeight;

    int scanSN;//表示当前正在进行的扫描序列数，从0开始

private:
    Ui::MainWindow *ui;
    CameraCalibration *calibrator;
    BlobDetector *blob;
    GrayCodes *grayCode;
    MultiFrequency *mf;
    Projector *pj;

    MFReconstruct *mfr;

    DaHengCamera *DHC;
    cv::Mat triggerLeftTemp[22];//Rom temp for saving images captured by trigger mode
    cv::Mat triggerRightTemp[22];

    bool usebc;//是否使用Basler相机
    bool showFocus;//是否显示对焦辅助窗口

    ///表示编码及解码重建方法，依次为：经典横竖条纹格雷编码，竖条纹格雷码+极线校正，多频外差条纹+极线校正
    enum codePattern{GRAY_ONLY, GRAY_EPI, MULTIFREQ_EPI};
    codePattern codePatternUsed;

    void createConnections();
    void createCentralWindow(QWidget *parent);
    void captureImage(QString pref, int saveCount, bool dispaly);
    void findPoint();
    void paintPoints();
    void getScreenGeometry();
    void closeCamera();
    void saveExImgs();
    void resetCamera();//在外触发采集后将相机恢复至正常采集状态
    void generatePath(int type);

    ///---------------辅助功能---------------///

    void progressPop(int up);
    void drawCross(QPainter &p, int x, int y);

    QTimer *timer;
    QImage image_1;
    QImage image_2;
    cv::Mat matShow_1;
    cv::Mat matShow_2;
    QPixmap pimage_1;//由图像指针得到的.png格式图像
    QPixmap pimage_2;

    bool isProjectorOpened;
    bool isConfigured;
    int saveCount;//count the photo captured.

    QString path_1;
    QString path_2;

    ///与set对话框有关的变量
    int black_ ;
    int white_;
    bool isAutoContrast;
    bool isRaySampling;
    bool isExportObj;
    bool isExportPly;
    bool haveColor;

private slots:
    void newproject();
    void openproject();

    void opencamera();
    void startfocusassistant();
    void closefocus();
    void setexposure();
    void showframe();
    void getframe();

    void usebasler();

    void selectPath(int PATH);

    void capturecalib();
    void redocapture();
    void projectorcontrol();

    void calib();
    void calibration();

    void scan();
    void pointmatch();
    void refindmatch();
    void resetfind();
    void showhidemanual();
    void finishmanualmatch();
    void startscan();
    void stopscan();
    void test();

    void reconstruct();
    void startreconstruct();

    void set();
    void getSetInfo();

    void changePointSize(int psize);
    void loadTestModel();
    void switchlanguage();

    void exitApp();

};

#endif // MAINWINDOW_H
