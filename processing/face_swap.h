#ifndef FACESWAPGENERATOR_H
#define FACESWAPGENERATOR_H

#include "iprocessingitem.h"

class FaceSwapGenerator : public IProcessingItem
{
public:
    FaceSwapGenerator();
    ~FaceSwapGenerator();

    int initialize(QString faceSrcImage, int faceSrcDownsampleRatio=1);
    void process(ProcessingResult result);

private:
    cv::Mat swapFace(cv::Mat &dstFaceImage, std::vector<cv::Point> dstFacePoints,dlib::rectangle dstFaceRect);
    cv::Mat getAffineTransformationMatrix(std::vector<cv::Point> srcFacePoints,std::vector<cv::Point> dstFacePoints);
    cv::Mat getFaceMask(cv::Mat& face,std::vector<cv::Point> facePoints);
    void equalizeFaceColors(const cv::Mat source_image, cv::Mat target_image, cv::Mat& mask);

    int  openSrcFaceImage(QString faceSrcImage);
    int  readSrcFaceKeypoints();
    void alignSrcFace(cv::Mat& faceImage, std::vector<cv::Point> &facePoints);


    QString m_face_src_image_path;
    QString m_face_shape_desc_path;

    dlib::frontal_face_detector m_src_face_detector;
    dlib::shape_predictor m_src_face_pose_model;
    int m_face_src_downsample_ratio;
    cv::Mat m_face_src_img;
    std::vector<cv::Point2i> m_face_src_points;
};

#endif // FACESWAPGENERATOR_H
