#include "mfreconstruct.h"
#include <QMessageBox>
#include <QApplication>

bool processl = true;
double PI = 3.141593;

MFReconstruct::MFReconstruct(QObject *parent) :
    QObject(parent)
{
    mask = NULL;
    decRows = NULL;
    decCols = NULL;
    points3DProjView = NULL;
//    cameras = new  VirtualCamera[2];//生成virtualcamera的两个实例，保存在数组cameras[2]
    camsPixels = new cv::vector<double>*[2];
    calibFolder = new QString[2];
    scanFolder = new QString[2];
    imgPrefix = new QString[2];
    pathSet = false;
    imgSuffix = ".png";//这里暂时认为图片后缀为.png
    numOfColBits = 12;
    numberOfImgs = 14;
}

MFReconstruct::~MFReconstruct()
{
    delete sr;
}


bool MFReconstruct::getParameters(int scansn, int camw, int camh, int blackt, bool havecolor, QString savePath)
{
    scanSN = scansn;

    cameraWidth = camw;
    cameraHeight = camh;
    blackThreshold = blackt;
    savePath_ = savePath;//equal to projectPath
    haveColor = havecolor;

    sr = new stereoRect(this, savePath, cv::Size(camw,camh));
    sr->getParameters();

    for (int i = 0; i < 2; i++){
        camsPixels[i] = NULL;
        QString pathI;
        if (i == 0){
            pathI = savePath + "/scan/left/";//Load Images for reconstruction
        }
        else{
            pathI = savePath + "/scan/right/";
        }
        scanFolder[i] = pathI;
        if (i == 0){
            imgPrefix[i] = QString::number(scanSN) + "/L";
            calibFolder[i] = savePath + "/calib/left/";
        }
        else{
            imgPrefix[i] = QString::number(scanSN) +"/R";
            calibFolder[i] = savePath + "/calib/right/";
        }
    }
    pathSet = true;

//    if (!loadCameras()){
//        QMessageBox::warning(NULL,tr("Get Param"),tr("Load Calibration files failed."));
//        return false;
//    }
//    else
        return true;
}


//bool MFReconstruct::loadCameras()//Load calibration data into camera[i]
//{
//    bool loaded;
//    for(int i = 0; i < 2; i++)
//    {
//        QString path;
//        path = calibFolder[i];
//#ifndef USE_STEREOCALIB_DATA
//        path += "cam_matrix.txt";
//        loaded = cameras[i].loadCameraMatrix(path);//defined in visualcamera
//        if(!loaded)
//            break;

//        path = calibFolder[i];
//        path += "cam_distortion.txt";
//        cameras[i].loadDistortion(path);//注意loaddistortion方法加载一个5X1矩阵
//#else
//        path += "cam_stereo.txt";
//        loaded = cameras[i].loadCameraMatrix(path);//defined in visualcamera
//        if(!loaded)
//            break;
//        path = calibFolder[i];
//        path += "distortion_stereo.txt";
//        cameras[i].loadDistortion(path);
//#endif
//        path = calibFolder[i];
//        path += "cam_rotation_matrix.txt";
//        cameras[i].loadRotationMatrix(path);

//        path = calibFolder[i];
//        path += "cam_trans_vectror.txt";
//        cameras[i].loadTranslationVector(path);

//        path = savePath_;
//        path += "/calib/fundamental_stereo.txt";//测试表明，采用立体标定得到的F效果好于单独标定得到的
//        cameras[i].loadFundamentalMatrix(path);

//        cameras[i].height = cameraHeight;
//        cameras[i].width = cameraWidth;
//    }
//    return loaded;
//}


bool MFReconstruct::loadCamImgs(QString folder, QString prefix, QString suffix)//load camera images
{
    cv::Mat tmp;
    if(!camImgs.empty())
        unloadCamImgs();

    sr->calParameters();

    for(int i = 0; i < numberOfImgs; i++){
        QString path;
        path = folder;//这里folder要达到left/right一层
        path += prefix + QString::number(i) + suffix;
        tmp.release();

        tmp = cv::imread(path.toStdString(),0);//flag=0 Return a grayscale image

        if(tmp.empty()){
            QMessageBox::warning(NULL,tr("Load Images"),tr("Scan Images not found!"));
            break;
        }
        else{
            if (processl){//第一次调用loadImg时认为是加载左相机图像
                sr->doStereoRectify(tmp,true);
                //cv::imwrite(path.toStdString(),tmp);
            }
            else{
                sr->doStereoRectify(tmp,false);
                //cv::imwrite(path.toStdString(),tmp);
            }
            ///这里对图像进行归一化操作，以细化图像灰度细节
            cv::Mat out = cv::Mat(tmp.rows,tmp.cols,CV_64FC1);
            Utilities::matToGray(tmp,out);
            camImgs.push_back(out);
//            cv::normalize(out,out,255.0,0.0,cv::NORM_MINMAX);
//            cv::imwrite(savePath_.toStdString()+"/q.png",out);
        }
    }
    processl = !processl;//每调用一次loadCamImgs都对“当前是否处理左图像”取反
    return !tmp.empty();
}


void MFReconstruct::unloadCamImgs()//unload camera images
{
    if(camImgs.size()){
        for(int i = 0; i<numberOfImgs; i++){
            camImgs[i].release();
        }
    }
    camImgs.clear();
}


bool MFReconstruct::runReconstruction()
{
    bool runSucess = false;

    ///加载相机标定参数及所采集图像
    for(int i = 0; i < 2; i++){
        camsPixels[i] = new cv::vector<double>[cameraHeight * cameraWidth];//将每个相机图像中的每个像素在投影区域中的横坐标记录

        runSucess = loadCamImgs(scanFolder[i], imgPrefix[i], imgSuffix);

        ///对指针存储器camsPixels赋值，camsPixels[i]的地址就是camPixel的地址
        camPixels = camsPixels[i];

        ///截至这一步，实例camera的position、width、height属性已被赋值，camera对应cameras[i]
        if(!runSucess)//如果加载图片失败，中断
            break;
        else{
            computeShadows();
            decodePatterns();
            unloadCamImgs();
        }
    }

    if(runSucess){
        points3DProjView = new PointCloudImage(cameraWidth, cameraHeight, haveColor); //最后一个bool值代表是否上色，这里改为false
        triangulation(camsPixels[0],camsPixels[1]);
    }
    return runSucess;
}


void MFReconstruct::computeShadows()
{
    int w = cameraWidth;
    int h = cameraHeight;
    mask.release();
    mask = cv::Mat(h, w, CV_8U,cv::Scalar(0));//注意h=行数rows，w=列数cols
    for(int col = 0; col < w; col++){
        for(int row = 0; row < h; row++){
            float blackVal, whiteVal;
            blackVal  = Utilities::matGet2D(camImgs[1], row, col);//camImgs[1]表示全黑图像
            whiteVal  = Utilities::matGet2D(camImgs[0], row, col);//camImgs[0]表示全白图像
//            QMessageBox::information(NULL,"fff",QString::number(blackVal) + QString::number(whiteVal));
            ///同一像素点在全黑、全白投影下反差大于blackThreshold，说明该点不在阴影里
            /// 注意图像已被归一化，因此blackThreshold需要除255
            if(whiteVal - blackVal > 0.1 && whiteVal > 0.4){
//                QMessageBox::information(NULL,"fff",QString::number(blackVal) + QString::number(whiteVal));
                Utilities::matSet2D(mask, row, col, 1);
            }
            else
                Utilities::matSet2D(mask, row, col, 0);
        }
    }
}


void MFReconstruct::decodePatterns()
{
    int w = cameraWidth;
    int h = cameraHeight;

    cv::Mat out(h,w,CV_64FC1);

    for(int row = 0; row < h; row++){
        for(int col = 0; col < w; col++){

            double phase = 0.0;//表示图像上(w,h)点在相位展开图上的相位值，每次循环进行一次初始化
            getPhase(row, col, phase);

            if(mask.at<uchar>(row, col)){
                ///if the pixel is not shadow, it's mask = 1, reconstruct
                out.at<double>(row,col) = phase;
                camPixels[(row*cameraWidth+col)].push_back(phase);
            }
            else
                out.at<double>(row,col) = 0;
        }
    }

    ///输出相位图
    cv::normalize(out,out,255.0,0.0,cv::NORM_MINMAX);
    if (!processl)
        cv::imwrite(savePath_.toStdString()+"/L.png",out);
    else
        cv::imwrite(savePath_.toStdString()+"/R.png",out);
}


void MFReconstruct::getPhase(int row, int col, double &phase)
{
    double P[3];//三组相对相位
    double P12, P23, P123;//展开相位

    ///prosses column images
    for(int count = 0; count < 3; count++){//3表示共进行3次4步相移
        double G1, G2, G3, G4;//点在四步相移图像中的灰度值
        G1 = Utilities::matGet2D(camImgs[4*count + 2], row, col);
        G2 = Utilities::matGet2D(camImgs[4*count + 3], row, col);
        G3 = Utilities::matGet2D(camImgs[4*count + 4], row, col);
        G4 = Utilities::matGet2D(camImgs[4*count + 5], row, col);

        ///计算相对相位，注意结果是弧度制还是角度制，加PI使取值范围为
        /// 0~2PI
        if (G4 == G2 && G1 > G3)
            P[count] = 0;
        else if (G4 == G2 && G1 < G3)
            P[count] = PI;
        else if (G1 == G3 && G4 > G2)
            P[count] = PI/2;
        else if (G1 == G3 && G4 < G2)
            P[count] = 3*PI/2;
        else if (G1 == G3 && G4 == G2)
            Utilities::matSet2D(mask, row, col, 0);
        else if (G1 < G3)
            P[count] = atan(double((G4-G2)/(G1-G3))) + PI;
        else if (G1 > G3 && G4 > G2)
            P[count] = atan(double((G4-G2)/(G1-G3)));
        else if (G1 > G3 && G4 < G2)
            P[count] = atan(double((G4-G2)/(G1-G3))) + 2*PI;
    }

    ///将相对相位利用外差原理进行相位展开
    P12 = (P[0] > P[1])?(P[0] - P[1]):(P[0] - P[1] + 2*PI);
    P23 = (P[1] > P[2])?(P[1] - P[2]):(P[1] - P[2] + 2*PI);
    P123 = (P12 > P23)?(P12 - P23):(P12 - P23 + 2*PI);
//    phase = P123;

    ///回到第二频率求解绝对相位
    double PM12 = P12 + round((P123*6 - P12)/(2*PI))*2*PI;//lamda12 = 6
    phase = P[0] + round((PM12*64/6 - P[0])/(2*PI))*2*PI;
//    phase = PM12;
}


void MFReconstruct::triangulation(cv::vector<double> *cam1Pixels, cv::vector<double> *cam2Pixels)
{
    int width = cameraWidth;
    int height = cameraHeight;

    cv::Mat matCoordTrans(3,4,CV_32F);//定义变换矩阵将当前次扫描坐标系对齐至首次扫描坐标系
    if (scanSN > 0){
        ///加载刚体变换矩阵
        QString loadPath = savePath_ + "/scan/transfer_mat" + QString::number(scanSN) + ".txt";
        VirtualCamera *cam = new VirtualCamera();
        cam->loadMatrix(matCoordTrans, 3, 4, loadPath.toStdString());
        delete cam;
    }

    /*
    ///遍历图像纵坐标
    for (int i = 0; i < height;i++){//<

        int kstart = 0;//表示每次遍历k(右图像横坐标)时的起点，在k循环找到匹配后更新为匹配k值
        int lastMatchedJ = 0;//表示匹配成功的上一个J值

        ///遍历左图像横坐标
        for (int j = 0;j < width;j++){//<<
            cv::vector<double> cam1Pix = cam1Pixels[i * width + j];//获得左图像该点相位值，注意cam1Pix是一个向量

            if (cam1Pix.size() == 0)//左图像该点未被赋相位值，略过
                continue;

            int bestK;
            bool rightMatched = false;//表示该行最左侧点是否已经匹配，即bestK是否存在
            double epixel = 0.1;//表示左右像素点相位值的初始匹配偏差，每处理一个j点刷新一次

            ///遍历右图像横坐标
            for (int k = kstart;k < width;k++){//<<<
                cv::vector<double> cam2Pix = cam2Pixels[i * width + k];

                if (cam2Pix.size() == 0)//右图像该点未被赋相位值，略过
                    continue;

                double eCurrentJ = fabs(cam1Pix[0] - cam2Pix[0]);
                if (eCurrentJ <= epixel){
                    ///说明左相机(j,i)点与右相机(k,i)点匹配误差较原有值小，则(k,i)点更有可能是(j,i)点的匹配点，故将
                    /// k存入最佳匹配缓存
                    bestK = k;
                    epixel = eCurrentJ;
                    rightMatched = true;
                }
                else{
                    ///当相位误差有增大趋势时，由相位的单调变化规律可知，k点之后不可能再有匹配点，故break
                    kstart = k;
                    break;
                }
            }//>>>

            if (rightMatched){
                ///对可能出现的左图像点误匹配右图像点情况进行处理
                if (lastMatchedJ == 0){
                    if (bestK - kstart > 1){
                        kstart = 2*kstart - bestK;//将kstart恢复至误匹配以前的状态
                        rightMatched = false;
                        break;//跳出j循环
                    }
                }
                else{
                    if (bestK - kstart > j - lastMatchedJ + 1){
                        kstart = 2*kstart - bestK;//将kstart恢复至误匹配以前的状态
                        rightMatched = false;
                        break;//跳出j循环
                    }
                }

                lastMatchedJ = j;
                ///以左图像该点二维坐标、对应点视差构建该点二维齐次坐标
                double point2D[] = {j, i, j - bestK, 1};//二维坐标
                cv::Mat p2D = cv::Mat(4,1,CV_64F,point2D);//构建坐标矩阵
                cv::Mat p3D;
                p3D = sr->Q * p2D;//此处调试以观察是否正确计算
                double x = p3D.at<double>(0,0);
                double y = p3D.at<double>(1,0);
                double z = p3D.at<double>(2,0);
                double w = p3D.at<double>(3,0);
                double ax = x/w;
                double ay = y/w;
                double az = z/w;

                cv::Point3f interPoint(ax,ay,az);
                cv::Point3f refinedPoint;

                ///以下判断为多次重建得到的点云拼接做准备
                if (scanSN > 0){
                    float point[] = {interPoint.x, interPoint.y, interPoint.z, 1};
                    cv::Mat pointMat(4, 1, CV_32F, point);
                    cv::Mat refineMat(3, 1, CV_32F);
                    refineMat = matCoordTrans * pointMat;
                    refinedPoint.x = refineMat.at<float>(0, 0);
                    refinedPoint.y = refineMat.at<float>(1, 0);
                    refinedPoint.z = refineMat.at<float>(2, 0);
                }
                else
                    refinedPoint = interPoint;
                points3DProjView->addPoint(i, j, refinedPoint);
            }

        }//>>
        //        qApp->processEvents();//防止运行过程中无响应，可能影响处理速度，暂时屏蔽
    }//>

    */

    ///遍历图像纵坐标
    for (int i = 0; i < height;i++){//<

        ///遍历左图像横坐标
        for (int j = 0;j < width;j++){//<<
            cv::vector<double> cam1Pix = cam1Pixels[i * width + j];//获得左图像该点相位值，注意cam1Pix是一个向量

            if (cam1Pix.size() == 0)//左图像该点未被赋相位值，略过
                continue;

            int bestK;
            bool rightMatched = false;//表示该行最左侧点是否已经匹配，即bestK是否存在
            double epixel = 1.0;//表示左右像素点相位值的初始匹配偏差，每处理一个j点刷新一次

            ///遍历右图像横坐标
            for (int k = 0;k < width;k++){//<<<
                cv::vector<double> cam2Pix = cam2Pixels[i * width + k];

                if (cam2Pix.size() == 0)//右图像该点未被赋相位值，略过
                    continue;

                double eCurrentJ = fabs(cam1Pix[0] - cam2Pix[0]);
                if (eCurrentJ <= epixel){
                    ///说明左相机(j,i)点与右相机(k,i)点匹配误差较原有值小，则(k,i)点更有可能是(j,i)点的匹配点，故将
                    /// k存入最佳匹配缓存
                    bestK = k;
                    epixel = eCurrentJ;
                    rightMatched = true;
                }
                else{
                    ///当相位误差有增大趋势时，由相位的单调变化规律可知，k点之后不可能再有匹配点，故break
                    if (rightMatched)
                        break;
                }
            }//>>>

            if (rightMatched){
                ///以左图像该点二维坐标、对应点视差构建该点二维齐次坐标
                double point2D[] = {j, i, j - bestK, 1};//二维坐标
                cv::Mat p2D = cv::Mat(4,1,CV_64F,point2D);//构建坐标矩阵
                cv::Mat p3D;
                p3D = sr->Q * p2D;//此处调试以观察是否正确计算
                double x = p3D.at<double>(0,0);
                double y = p3D.at<double>(1,0);
                double z = p3D.at<double>(2,0);
                double w = p3D.at<double>(3,0);
                double ax = x/w;
                double ay = y/w;
                double az = z/w;

                cv::Point3f interPoint(ax,ay,az);
                cv::Point3f refinedPoint;

                ///以下判断为多次重建得到的点云拼接做准备
                if (scanSN > 0){
                    float point[] = {interPoint.x, interPoint.y, interPoint.z, 1};
                    cv::Mat pointMat(4, 1, CV_32F, point);
                    cv::Mat refineMat(3, 1, CV_32F);
                    refineMat = matCoordTrans * pointMat;
                    refinedPoint.x = refineMat.at<float>(0, 0);
                    refinedPoint.y = refineMat.at<float>(1, 0);
                    refinedPoint.z = refineMat.at<float>(2, 0);
                }
                else
                    refinedPoint = interPoint;
                if (haveColor){
                    int val = camImgs[0].at<uchar>(i,j);
                    cv::Vec3i graycolor = cv::Vec3i(val,val,val);
                    points3DProjView->addPoint(j, i, refinedPoint, graycolor);
                }
                else
                    points3DProjView->addPoint(j, i, refinedPoint);
//                if (!addp)
//                    QMessageBox::information(NULL,"fff","Add Point fail");
            }

        }//>>
        //        qApp->processEvents();//防止运行过程中无响应，可能影响处理速度，暂时屏蔽
    }//>

}

