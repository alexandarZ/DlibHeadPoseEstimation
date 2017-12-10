#ifndef FACE_SNAPCHATDOG_H
#define FACE_SNAPCHATDOG_H

#include "processing/iprocessingitem.h"

class FaceSnapchatDog : public IProcessingItem
{
public:
    FaceSnapchatDog();
    ~FaceSnapchatDog();

    int initialize();
    void process(ProcessingResult result);

    cv::Mat processEars(cv::Mat &frame, dlib::full_object_detection &faceKeypoints);
    cv::Mat processNose(cv::Mat &frame, dlib::full_object_detection &faceKeypoints);
    cv::Mat processTongue(cv::Mat &frame, dlib::full_object_detection &faceKeypoints);

private:
    QString m_imgears_path,m_imgnose_path,m_imgtongue_path;
    cv::Mat m_ears_img,m_nose_img,m_tongue_img;
    float angle;
};

#endif // FACE_SNAPCHATDOG_H
