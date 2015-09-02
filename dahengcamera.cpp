#include "dahengcamera.h"
#include "qmessagebox.h"

const int XStart = 0;//图像左上角点在相机幅面1280X1024上相对于幅面左上角点坐标
const int YStart = 0;
HV_RESOLUTION Resolution;
int ADCLevel = ADC_LEVEL2;


DaHengCamera::DaHengCamera(QObject *parent) :
    QObject(parent)
{
    cameraOpened = false;
}

DaHengCamera::~DaHengCamera()
{
    if(cameraOpened){
        OnSnapexStop();
        OnSnapexClose();
        HVSTATUS status = STATUS_OK;
        //	关闭数字摄像机，释放数字摄像机内部资源
        status = EndHVDevice(m_hhv_1);
        status = EndHVDevice(m_hhv_2);
        //	回收图像缓冲区
        delete []m_pRawBuffer_1;
        delete []m_pRawBuffer_2;
    }
}

///---------------------相机-----------------------///
void DaHengCamera::daHengExposure(int leftexposure, int rightexposure)
{
    switch (leftexposure) {
    case 0:
        ADCLevel = ADC_LEVEL3;
        break;
    case 1:
        ADCLevel = ADC_LEVEL2;
        break;
    case 2:
        ADCLevel = ADC_LEVEL1;
        break;
    case 3:
        ADCLevel = ADC_LEVEL0;
        break;
    }
    HVADCControl(m_hhv_1, ADC_BITS, ADCLevel);
    switch (rightexposure) {
    case 0:
        ADCLevel = ADC_LEVEL3;
        break;
    case 1:
        ADCLevel = ADC_LEVEL2;
        break;
    case 2:
        ADCLevel = ADC_LEVEL1;
        break;
    case 3:
        ADCLevel = ADC_LEVEL0;
        break;
    }
    HVADCControl(m_hhv_2, ADC_BITS, ADCLevel);
}

void DaHengCamera::initialDaHengCamera(int camerawidth, int cameraheight,bool extrigger)
{
    cam_w = camerawidth;
    cam_h = cameraheight;

    ///----------打开相机------------/
    if (!cameraOpened){
        HVSTATUS status_1 = STATUS_OK;
        HVSTATUS status_2 = STATUS_OK;
        m_pRawBuffer_1	= NULL;
        m_pRawBuffer_2	= NULL;

        status_1 = BeginHVDevice(1, &m_hhv_1);
        status_2 = BeginHVDevice(2, &m_hhv_2);
        if(status_1 == STATUS_OK && status_2 == STATUS_OK)
            cameraOpened = true;
        else{
            cameraOpened = false;
            QMessageBox::warning(NULL, tr("Cameras not found"), tr("Make sure two Daheng cameras have connected to the computer."));
            return;
        }

        ///----------设置相机分辨率----------/
        switch (camerawidth) {
        case 1280:
            Resolution = RES_MODE0;
            break;
        case 640:
            Resolution = RES_MODE1;
            break;
        case 320:
            Resolution = RES_MODE2;
            break;
        }
        HVSetResolution(m_hhv_1, Resolution);//Set the resolution of cameras
        HVSetResolution(m_hhv_2, Resolution);

        ///--------设置触发模式----------/
        if (extrigger){
            HVSetSnapMode(m_hhv_1, TRIGGER);//Snap mode include CONTINUATION、TRIGGER
            HVSetSnapMode(m_hhv_2, TRIGGER);
        }
        else{
            HVSetSnapMode(m_hhv_1, CONTINUATION);
            HVSetSnapMode(m_hhv_2, CONTINUATION);
        }


        HVADCControl(m_hhv_1, ADC_BITS, ADCLevel);//设置ADC的级别
        HVADCControl(m_hhv_2, ADC_BITS, ADCLevel);

        HVTYPE type = UNKNOWN_TYPE;//获取设备类型
        int size    = sizeof(HVTYPE);
        HVGetDeviceInfo(m_hhv_1,DESC_DEVICE_TYPE, &type, &size);//由于两相机型号相同，故只获取一个

        HVSetOutputWindow(m_hhv_1, XStart, YStart, camerawidth, cameraheight);
        HVSetOutputWindow(m_hhv_2, XStart, YStart, camerawidth, cameraheight);

        ///---------设置采集速度---------/
        HVSetSnapSpeed(m_hhv_1, HIGH_SPEED);
        HVSetSnapSpeed(m_hhv_2, HIGH_SPEED);

        ///--------设置速度级别---------/
//        DWORD dwSpeed = 13; //单位ms？
//        int nLen = sizeof(DWORD);
//        HVCommand(m_hhv_1,CMD_SET_SNAPSPEED_COEFFICIENT, &dwSpeed, &nLen);
//        HVCommand(m_hhv_2,CMD_SET_SNAPSPEED_COEFFICIENT, &dwSpeed, &nLen);


        m_pRawBuffer_1 = new BYTE[camerawidth * cameraheight];
        m_pRawBuffer_2 = new BYTE[camerawidth * cameraheight];
    }

}

void DaHengCamera::openDaHengCamera()
{
    OnSnapexOpen();
    OnSnapexStart();
}

void DaHengCamera::daHengSnapShot(int camNo)
{
    (camNo==1)?(HVSnapShot(m_hhv_1, &m_pRawBuffer_1, 1)):(HVSnapShot(m_hhv_2, &m_pRawBuffer_2, 1));
}

void DaHengCamera::OnSnapexOpen()
{
    HVSTATUS status = STATUS_OK;
    status = HVOpenSnap(m_hhv_1, SnapThreadCallback, this);
    status = HVOpenSnap(m_hhv_2, SnapThreadCallback, this);
}

void DaHengCamera::OnSnapexStart()
{
    HVSTATUS status = STATUS_OK;
    status = HVStartSnap(m_hhv_1, &m_pRawBuffer_1,1);
    status = HVStartSnap(m_hhv_2, &m_pRawBuffer_2,1);
}

void DaHengCamera::OnSnapexStop()
{
    HVSTATUS status = STATUS_OK;
    status = HVStopSnap(m_hhv_1);
    status = HVStopSnap(m_hhv_2);
}

void DaHengCamera::OnSnapexClose()
{
    HVSTATUS status = STATUS_OK;
    status = HVCloseSnap(m_hhv_1);
    status = HVCloseSnap(m_hhv_2);
}

void DaHengCamera::closeCamera()
{
    OnSnapexStop();
    OnSnapexClose();

    HVSTATUS status = STATUS_OK;
    //	关闭数字摄像机，释放数字摄像机内部资源
    status = EndHVDevice(m_hhv_1);
    status = EndHVDevice(m_hhv_2);

//    //	回收图像缓冲区
//    delete []m_pRawBuffer_1;
//    delete []m_pRawBuffer_2;

    cameraOpened = false;
}

int CALLBACK DaHengCamera::SnapThreadCallback(HV_SNAP_INFO *pInfo)
{
    DaHengCamera *This = (DaHengCamera *)(pInfo->pParam);
    if (pInfo->nDevice==1)
        emit This->sendImage();//可以用但很慢且卡
    return 1;
}

