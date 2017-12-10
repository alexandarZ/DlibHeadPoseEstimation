#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include "iprocessingitem.h"

#define DOWNSAMPLE_RATIO 1.5

class FaceDetector : public IProcessingItem
{
public:
    FaceDetector();
    ~FaceDetector();

    int initialize(float face_downsample_ratio=2.0);
    void process(ProcessingResult result);

private:

    void renderFacePoints(cv::Mat& img,dlib::full_object_detection &face);

    dlib::frontal_face_detector m_face_detector;
    dlib::shape_predictor m_pose_model;
    std::vector<dlib::rectangle> m_faces;
    float m_face_downsample_ratio;
};

#endif // FACEDETECTOR_H
