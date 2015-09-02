#ifndef MESHCREATOR_H
#define MESHCREATOR_H

#include <QObject>
#include "pointcloudimage.h"
#include <QString>

class MeshCreator : public QObject
{
    Q_OBJECT
public:
    MeshCreator(QObject *parent = 0, PointCloudImage *in = NULL);
    ~MeshCreator();
    void exportObjMesh(QString path);
    void exportPlyMesh(QString path);
private:
    int *pixelNum;
    PointCloudImage *cloud;
    int MeshCreator::access(int i,int j);
    int MeshCreator::access(int i,int j, int z);

    int w;
    int h;
};

#endif // MESHCREATOR_H
