#ifndef PLYLOADER_H
#define PLYLOADER_H

#include <QObject>
#include <QFile>
#include <QFileDevice>
#include <iostream>
#include <vector>

using namespace std;

class PlyLoader : public QObject
{
    Q_OBJECT
public:
    PlyLoader(QObject *parent = 0);
    ~PlyLoader();

    bool LoadModel(QString filename);
    float* mp_vertexXYZ;
    int m_totalConnectedPoints;
};

#endif // PLYLOADER_H
