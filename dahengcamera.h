#ifndef DAHENGCAMERA_H
#define DAHENGCAMERA_H

#include <QObject>

#include "Windows.h"//加载此头文件以解决大恒相机头文件类型未定义问题
#include <HVDAILT.h>
#include <Raw2Rgb.h>

class DaHengCamera : public QObject
{
    Q_OBJECT
public:
    DaHengCamera(QObject *parent = 0);
    ~DaHengCamera();

    bool cameraOpened;

    void daHengExposure(int leftexposure, int rightexposure);
    void initialDaHengCamera(int camerawidth, int cameraheight, bool extrigger);
    void openDaHengCamera();
    void daHengSnapShot(int camNo);
    void closeCamera();

    BYTE *m_pRawBuffer_1;
    BYTE *m_pRawBuffer_2;

    int dwLastStatus;

private:

    HHV	m_hhv_1;
    HHV	m_hhv_2;

    int cam_w;
    int cam_h;

    static int CALLBACK SnapThreadCallback(HV_SNAP_INFO *pInfo);//static不可去掉

    ///---------------相机相关函数---------------///
    void OnSnapexOpen();
    void OnSnapexStart();
    void OnSnapexStop();
    void OnSnapexClose();

signals:
    void sendImage();

};

#endif // DAHENGCAMERA_H
