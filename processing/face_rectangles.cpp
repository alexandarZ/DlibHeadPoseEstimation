#include "face_rectangles.h"
#include <QDebug>

FaceRectangles::FaceRectangles()
{
    this->m_enabled = false;
}

FaceRectangles::~FaceRectangles()
{

}

int FaceRectangles::initialize()
{
    return 0;
}

void FaceRectangles::process(ProcessingResult result)
{
    if(this->m_enabled)
    {
        for(int i=0;i<result.face_rectangles.size();i++)
        {
            dlib::rectangle r = result.face_rectangles[i];
            cv::rectangle(result.camera_frame,cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1)),cv::Scalar(0,255,0),2);
        }
    }

    IProcessingItem::process(result);
}

