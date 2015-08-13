#include "manualmatch.h"
#include "ui_manualmatch.h"

#include <QImage>
#include <QPainter>
#include <QMessageBox>

bool modified = false;//表示初始值是否被修改
QFont textfont("Calibri",50);
QColor gcolor(0,255,0);
QColor rcolor(255,0,0);

ManualMatch::ManualMatch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualMatch)
{
    ui->setupUi(this);

    connect(ui->confirmButton,SIGNAL(clicked()),this,SLOT(confirmID()));
    connect(ui->finishButton,SIGNAL(clicked()),this,SLOT(finish()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(hide()));
    connect(ui->resetButton,SIGNAL(clicked()),this,SLOT(reset()));
    connect(ui->deleteButton,SIGNAL(clicked()),this,SLOT(deletepoint()));

    onMark = 0;
}


ManualMatch::~ManualMatch()
{
    delete ui;
}

///
/// \brief ManualMatch::initialize
/// 根据输入的dotinorder和correspond生成一个用于显示及数据存储的库
///
void ManualMatch::initialize()
{
    if (!dotInfo.empty())
        dotInfo.clear();
    for (size_t i=0;i<dotInOrder.size();i++){
        cv::vector<int> temp;
        bool idexist = false;
        temp.push_back(dotInOrder[i][0].x);
        temp.push_back(dotInOrder[i][0].y);
        temp.push_back(dotInOrder[i][1].x);
        temp.push_back(dotInOrder[i][1].y);
        int ID;
        for (size_t c = 0; c < correspond.size(); c++){
            if (i == correspond[c].y){
                ID = correspond[c].x;
                idexist = true;
            }
        }
        if (idexist){
            temp.push_back(ID);
        }
        else{
            temp.push_back(-1);//如果序号i的点不为已知，则将ID位设为-1
        }
        temp.push_back(i);//置入序号,后续根据dotinfo中对应序号点是否存在判断其是否被删除
        dotInfo.push_back(temp);
    }
    totalSize = dotInfo.size();
}

void ManualMatch::setImage()
{
    ///Load images and initialize the painter
    QImage pimage_1 = QImage(leftImage.data,leftImage.cols,leftImage.rows,QImage::Format_Indexed8);
    QImage pimage_2 = QImage(rightImage.data,rightImage.cols,rightImage.rows,QImage::Format_Indexed8);
    QPixmap pcopy_1 =  QPixmap::fromImage(pimage_1);
    QPixmap pcopy_2 = QPixmap::fromImage(pimage_2);
    QPainter pt_1(&pcopy_1);
    QPainter pt_2(&pcopy_2);
    pt_1.setFont(textfont);
    pt_2.setFont(textfont);

    for(size_t i = 0;i < dotInfo.size();i++){
        pt_1.setPen(gcolor);
        pt_2.setPen(gcolor);

        drawCross(pt_1, dotInfo[i][0] ,dotInfo[i][1]);///绘制标记点绿十字，不论是否为已知点均用绿十字示出
        drawCross(pt_2, dotInfo[i][2], dotInfo[i][3]);

        if (dotInfo[i][4] != -1){//若值不为-1，说明为已知点，显示i点ID
            int ID = dotInfo[i][4];
            pt_1.drawText(dotInfo[i][0],dotInfo[i][1],QString::number(ID));
            pt_2.drawText(dotInfo[i][2], dotInfo[i][3],QString::number(ID));
        }
        else{//若为未知点，显示i点为问号
            pt_1.drawText(dotInfo[i][0],dotInfo[i][1],"?");
            pt_2.drawText(dotInfo[i][2], dotInfo[i][3],"?");
        }
    }

    ///用红色方框标记当前准备赋予编号的点
    pt_1.setPen(rcolor);
    pt_2.setPen(rcolor);
    pt_1.drawRect(dotInfo[onMark][0]-15,dotInfo[onMark][1]-15,30,30);
    pt_2.drawRect(dotInfo[onMark][2]-15,dotInfo[onMark][3]-15,30,30);

    ui->leftImage->setPixmap(pcopy_1);
    ui->rightImage->setPixmap(pcopy_2);

    ///如果onMark点已经被标记过，则在idEdit中显示ID值
    if (dotInfo[onMark][4] != -1){
        int ID = dotInfo[onMark][4];
        ui->idEdit->setText(QString::number(ID));
    }
    else
        ui->idEdit->clear();
}

void ManualMatch::confirmID()
{
    int id = ui->idEdit->text().toInt();
    dotInfo[onMark].at(4) = id;

    if (onMark == totalSize - 1)
        onMark = 0;
    else
        onMark++;

    ui->current->setText(QString::number(onMark));
    modified = true;
    setImage();//根据新的信息重绘图像
}

void ManualMatch::deletepoint()
{
    dotInfo.erase(dotInfo.begin()+onMark);

    if (onMark < totalSize - 1){
        onMark++;
    }
    else{
        onMark = 0;
    }
    setImage();
}

void ManualMatch::finish()
{
    for (size_t i=0;i<dotInfo.size();i++){
        if (dotInfo[i][4] != -1){
            cv::Point2i corr;
            corr.x = dotInfo[i][4];
            corr.y = i;
            refinedCorr.push_back(corr);
        }
    }
    for (size_t j=0;j<dotInOrder.size();j++){
        bool found = false;
        for (size_t k=0;k<dotInfo.size();k++){
            if (dotInfo[k][5] == j)
                found = true;
        }
        if (!found)
            abandoned.push_back(j);//j=4表示第5个值被删除
    }
    this->hide();
    emit outputdata();
}

void ManualMatch::reset()
{
    initialize();
    refinedCorr.clear();
    abandoned.clear();
    onMark = 0;
    setImage();
}

void ManualMatch::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Enter)
        confirmID();
    else if (e->key() == Qt::Key_Delete)
        deletepoint();
}

void ManualMatch::drawCross(QPainter &p, int x, int y)
{
    int len = 25;
    p.drawLine(x - len, y, x + len, y);
    p.drawLine(x, y - len, x, y + len);
}


bool ManualMatch::BelongTo(size_t e, cv::vector<int> C)
{
    for (size_t i = 0; i < C.size(); i++){
        if (e == C[i])
            return true;
    }
    return false;
}

