#ifndef FACE_FANCYMAN_H
#define FACE_FANCYMAN_H

#include "processing/iprocessingitem.h"

class FaceFancyMan : public IProcessingItem
{
public:
    FaceFancyMan();
    ~FaceFancyMan();

    int initialize();
    void process(ProcessingResult result);

    cv::Mat processHat(cv::Mat& frame,dlib::full_object_detection& faceKeypoints);
    cv::Mat processGlasses(cv::Mat& frame,dlib::full_object_detection& faceKeypoints);
    cv::Mat processMustaches(cv::Mat& frame,dlib::full_object_detection& faceKeypoints);

private:
    QString m_imghat_path,m_imgglasses_path,m_imgmustaches_path;
    cv::Mat m_hat_img,m_glasses_img,m_mustaches_img;
    int angle;
};

#endif // FACE_FANCYMAN_H
