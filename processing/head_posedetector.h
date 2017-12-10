#ifndef HEADPOSEDETECTOR_H
#define HEADPOSEDETECTOR_H

#include "iprocessingitem.h"

class HeadPoseDetector : public IProcessingItem
{
public:
    HeadPoseDetector();
    ~HeadPoseDetector();

    int initialize();
    void process(ProcessingResult result);

    void clearData();

private:

    void getEulerAngles(cv::Mat& cameraRotMatrix,cv::Vec3d& euler_angles);

    void renderHeadAngles(cv::Mat &image,cv::Mat& rotationVector,cv::Mat& translationVector,cv::Mat& cameraMatrix);

    int accumulator_yaw,accumulator_roll,accumulator_pitch;
};

#endif // HEADPOSEDETECTOR_H
