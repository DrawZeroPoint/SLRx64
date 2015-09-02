#ifndef BLOBDETECTOR_H
#define BLOBDETECTOR_H

#include <opencv/cv.h>
#include <QObject>

using namespace cv;

class BlobDetector : public QObject
{
    Q_OBJECT
public:
    BlobDetector(QObject *parent = 0);
    ~BlobDetector();

    void findBlobs(const cv::Mat &binaryImage, vector<Point2d> &centers) const;

};

#endif // BLOBDETECTOR_H
