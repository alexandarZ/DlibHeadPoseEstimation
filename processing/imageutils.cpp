#include "imageutils.h"
#define MATH_PI 3.14
#include <QDebug>

ImageUtils::ImageUtils()
{

}

double ImageUtils::calculateAngle(cv::Point pt1, cv::Point pt2)
{
    int x1 = pt1.x;
    int x2 = pt2.x;
    int y1 = pt1.y;
    int y2 = pt2.y;

    return -((180/MATH_PI) * atan((double(y2-y1))/double(x2-x1)));
}

cv::Mat ImageUtils::rotateImage(cv::Mat &image, double angle)
{
    cv::Mat result;

    // Get rotation matrix
    cv::Point center(image.cols/2.0f,image.rows/2.0f);
    cv::Mat rotMatrix = cv::getRotationMatrix2D(center,angle,1.0);

    cv::Rect bbox = cv::RotatedRect(center,image.size(), angle).boundingRect();

    // Adjust transformation matrix
    rotMatrix.at<double>(0,2) += bbox.width/2.0 - center.x;
    rotMatrix.at<double>(1,2) += bbox.height/2.0 - center.y;

    // Rotate
    cv::warpAffine(image,result,rotMatrix,bbox.size(),cv::INTER_LINEAR);

    return result;
}

cv::Mat ImageUtils::drawPngImage(cv::Mat &frame, cv::Mat &image, int x0, int y0)
{
    cv::Mat output;
    frame.copyTo(output);


    for(int y =y0; y < output.rows; ++y)
    {

        int fY = y - y0; // because of the translation

        // we are done of we have processed all rows of the foreground image.
        if(fY >= image.rows || fY <0)
            break;

        // start at the column indicated by location,

        // or at column 0 if location.x is negative.
        for(int x = x0; x < output.cols; ++x)
        {
            int fX = x - x0; // because of the translation.

            // we are done with this row if the column is outside of the foreground image.
            if(fX >= image.cols || fX <0)
                break;

            // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
            double opacity =((double)image.data[fY * image.step + fX * image.channels() + 3])/ 255.;

            // and now combine the background and foreground pixel, using the opacity,

            // but only if opacity > 0.
            for(int c = 0; opacity > 0 && c < frame.channels(); ++c)
            {
                unsigned char foregroundPx = image.data[fY * image.step + fX * image.channels() + c];
                unsigned char backgroundPx = frame.data[y * frame.step + x * frame.channels() + c];
                output.data[y*output.step + output.channels()*x + c] = backgroundPx * (1.-opacity) + foregroundPx * opacity;
            }
        }
    }

    return output;
}
