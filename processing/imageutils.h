#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>

class ImageUtils
{
public:
    ImageUtils();

    static double calculateAngle(cv::Point pt1,cv::Point pt2);
    static cv::Mat rotateImage(cv::Mat& image, double angle);
    static cv::Mat drawPngImage(cv::Mat& frame, cv::Mat& image, int x0, int y0);
};

#endif // IMAGEUTILS_H
