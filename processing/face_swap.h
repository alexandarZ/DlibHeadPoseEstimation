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
    cv::Mat swapFace(cv::Mat dstFaceImage,std::vector<cv::Point> dstFacePoints);

    int  openSrcFaceImage(QString faceSrcImage);
    int readSrcFaceKeypoints();

    void calculateDelaunayTriangles(cv::Rect rect, std::vector<cv::Point2f> &points, std::vector<std::vector<int>> &delaunayTri);
    void applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri, std::vector<cv::Point2f> &dstTri);
    void warpAndBlend(cv::Mat &img1, cv::Mat &img2, std::vector<cv::Point2f> &t1, std::vector<cv::Point2f> &t2);

    QString m_face_src_image_path;
    QString m_face_shape_desc_path;

    dlib::frontal_face_detector m_src_face_detector;
    dlib::shape_predictor m_src_face_pose_model;
    int m_face_src_downsample_ratio;
    cv::Mat m_face_src_img;
    cv::Mat m_face_src_img_warped;
    std::vector<cv::Point2f> m_face_src_points;
};

#endif // FACESWAPGENERATOR_H
