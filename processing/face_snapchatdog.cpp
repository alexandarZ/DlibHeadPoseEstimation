#include "face_snapchatdog.h"
#include "processing/imageutils.h"
#include <QApplication>

FaceSnapchatDog::FaceSnapchatDog()
{
    this->setEnabled(false);
    this->angle=0.0;
}

FaceSnapchatDog::~FaceSnapchatDog()
{

}

int FaceSnapchatDog::initialize()
{
    /* Initialize path */
    m_imgears_path   = QApplication::applicationDirPath()+"/overlays/doggy_ears.png";
    m_imgnose_path   = QApplication::applicationDirPath()+"/overlays/doggy_nose.png";
    m_imgtongue_path = QApplication::applicationDirPath()+"/overlays/doggy_tongue.png";

    /* Initialize images */
    m_ears_img   = cv::imread(m_imgears_path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);
    m_nose_img   = cv::imread(m_imgnose_path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);
    m_tongue_img = cv::imread(m_imgtongue_path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);

    /* Check images */
    if(!m_ears_img.data)
    {
        qDebug()<<"Error - Could not load snapchat dog ears image!";
        this->setEnabled(false);
        return 1;
    }

    if(!m_nose_img.data)
    {
        qDebug()<<"Error - Could not load snapchat dog nose image!";
        this->setEnabled(false);
        return 1;
    }

    if(!m_tongue_img.data)
    {
        qDebug()<<"Error - Could not load snapchat dog tongue image!";
        this->setEnabled(false);
        return 1;
    }

    qDebug()<<"Snapchat dog filter initialized successfully!";

    return 0;
}

void FaceSnapchatDog::process(ProcessingResult result)
{
    if(this->m_enabled)
    {
        for(int i=0;i<result.face_rectangles.size();i++)
        {
            if(result.face_shapes[i].num_parts()==68)
            {
                try
                {
                    cv::Mat frameWithEars   = processEars(result.camera_frame,result.face_shapes.at(i));
                    cv::Mat frameWithNose   = processNose(frameWithEars,result.face_shapes.at(i));
                    cv::Mat frameWithTongue = processTongue(frameWithNose,result.face_shapes.at(i));

                    result.camera_frame = frameWithTongue;
                }
                catch(...)
                {
                    qDebug()<<"Error - Could not set snapchat dog face overlay!";
                }
            }
        }
    }

    IProcessingItem::process(result);
}

cv::Mat FaceSnapchatDog::processEars(cv::Mat& frame, dlib::full_object_detection& faceKeypoints)
{
    cv::Mat ears_rotated,ears_resized, result;

    /* Calculate width and height for ears rectangle */

    int width    = (faceKeypoints.part(16).x() - faceKeypoints.part(0).x())+40;
    int height   = faceKeypoints.part(1).y()  - faceKeypoints.part(19).y();

    int x0       = faceKeypoints.part(19).x()-width/3+5;
    int y0       = faceKeypoints.part(19).y()-height-10;

    /* Calculate rotation angle */
    cv::Point pt1(faceKeypoints.part(31).x(),faceKeypoints.part(31).y());
    cv::Point pt2(faceKeypoints.part(35).x(),faceKeypoints.part(35).y());

    /* Rotate ears */
    double rotAngle = ImageUtils::calculateAngle(pt1,pt2);
    ears_rotated = ImageUtils::rotateImage(m_ears_img,rotAngle);

    /* Resize ears to fit */
    if(height<50)
    {
        height=50;
    }

    cv::Rect fRect(x0,y0,width,height);
    cv::resize(ears_rotated,ears_resized,cv::Size(fRect.width,fRect.height));

    /* Draw ears over frame */
    result = ImageUtils::drawPngImage(frame,ears_resized,x0,y0);

    return result;
}

cv::Mat FaceSnapchatDog::processNose(cv::Mat &frame, dlib::full_object_detection &faceKeypoints)
{
    cv::Mat nose_rotated,nose_resized, result;

    /* Calculate width and height for nose rectangle */

    int width    = (faceKeypoints.part(35).x() - faceKeypoints.part(32).x())+20;
    int height   = (faceKeypoints.part(51).y() - faceKeypoints.part(30).y())+5;
    int x0       = faceKeypoints.part(31).x()  - 5;
    int y_offset = faceKeypoints.part(51).y()  - faceKeypoints.part(33).y();
    int y0       = faceKeypoints.part(30).y()  - y_offset;

    /* Calculate rotation angle */
    cv::Point pt1(faceKeypoints.part(31).x(),faceKeypoints.part(31).y());
    cv::Point pt2(faceKeypoints.part(35).x(),faceKeypoints.part(35).y());

    /* Rotate nose */
    double rotAngle = ImageUtils::calculateAngle(pt1,pt2);
    nose_rotated = ImageUtils::rotateImage(m_nose_img,rotAngle);

    /* Resize nose to fit */
    cv::Rect fRect(x0,y0,width,height);
    cv::resize(nose_rotated,nose_resized,cv::Size(fRect.width,fRect.height));

    /* Draw nose over frame */
    result = ImageUtils::drawPngImage(frame,nose_resized,x0,y0);

    return result;
}

cv::Mat FaceSnapchatDog::processTongue(cv::Mat &frame, dlib::full_object_detection &faceKeypoints)
{
    cv::Mat tongue_rotated,tongue_resized, result;

    /* Check is mouth open */

    int mouthHeight = (faceKeypoints.part(57).y() - faceKeypoints.part(50).y());
    int mouthThreshold = (faceKeypoints.part(33).y() - faceKeypoints.part(30).y())+11;

    /* If mouth is not open, return */
    if(mouthThreshold > mouthHeight)
    {
        return frame;
    }

    /* Calculate width and height for tongue rectangle */
    int width    = (faceKeypoints.part(54).x() - faceKeypoints.part(48).x())+20;
    int height   = (faceKeypoints.part(57).y() - faceKeypoints.part(50).y())+10;

    int x0       = faceKeypoints.part(48).x()-5;
    int y0       = faceKeypoints.part(51).y()+10;

    /* Calculate rotation angle */
    cv::Point pt1(faceKeypoints.part(31).x(),faceKeypoints.part(31).y());
    cv::Point pt2(faceKeypoints.part(35).x(),faceKeypoints.part(35).y());

    /* Rotate tongue */
    double rotAngle = ImageUtils::calculateAngle(pt1,pt2);
    tongue_rotated = ImageUtils::rotateImage(m_tongue_img,rotAngle);

    /* Resize tongue to fit */
    cv::Rect fRect(x0,y0,width,height);
    cv::resize(tongue_rotated,tongue_resized,cv::Size(fRect.width,fRect.height));

    /* Draw tongue over frame */
    result = ImageUtils::drawPngImage(frame,tongue_resized,x0,y0);

    return result;
}
