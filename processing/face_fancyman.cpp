#include "face_fancyman.h"
#include "processing/imageutils.h"
#include <QApplication>

FaceFancyMan::FaceFancyMan()
{
    this->setEnabled(false);
    this->angle = 0;
}

FaceFancyMan::~FaceFancyMan()
{

}

int FaceFancyMan::initialize()
{
    /* Initialize path */
    m_imgglasses_path   =  QApplication::applicationDirPath()+"/overlays/glasses.png";
    m_imghat_path       =  QApplication::applicationDirPath()+"/overlays/hat.png";
    m_imgmustaches_path =  QApplication::applicationDirPath()+"/overlays/mustaches.png";

    /* Initialize images */
    m_hat_img       = cv::imread(m_imghat_path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);
    m_glasses_img   = cv::imread(m_imgglasses_path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);
    m_mustaches_img = cv::imread(m_imgmustaches_path.toStdString(),CV_LOAD_IMAGE_UNCHANGED);

    /* Check images */

    if(!m_hat_img.data)
    {
        qDebug()<<"Error - Could not load fancy man hat image ";
        return 1;
    }

    if(!m_glasses_img.data)
    {
        qDebug()<<"Error - Could not load fancy man glasses image!";
        return 1;
    }

    if(!m_mustaches_img.data)
    {
        qDebug()<<"Error - Could not load fancy man mustaches image!";
        return 1;
    }

    qDebug()<<"Fancy man filter initialized successfully!";
    return 0;
}

void FaceFancyMan::process(ProcessingResult result)
{
    if(this->m_enabled)
    {
        for(int i=0;i<result.face_rectangles.size();i++)
        {
            if(result.face_shapes[i].num_parts()==68)
            {
                try
                {
                    cv::Mat frameWithHat       = processHat(result.camera_frame,result.face_shapes.at(i));
                    cv::Mat frameWithGlasses   = processGlasses(frameWithHat,result.face_shapes.at(i));
                    cv::Mat frameWithMustaches = processMustaches(frameWithGlasses,result.face_shapes.at(i));

                    result.camera_frame = frameWithMustaches;
                }
                catch(...)
                {
                    qDebug()<<"Error - Could not set fancy man overlay images!";
                }
            }
        }
    }

    IProcessingItem::process(result);
}

cv::Mat FaceFancyMan::processHat(cv::Mat &frame, dlib::full_object_detection &faceKeypoints)
{
    cv::Mat hat_rotated,hat_resized, result;

    /* Calculate width and height for hat rectangle */

    int width    = (faceKeypoints.part(16).x() - faceKeypoints.part(0).x())+40;
    int height   = faceKeypoints.part(1).y()   - faceKeypoints.part(19).y()+60;

    int x0       = faceKeypoints.part(20).x() - width/3;
    int y0       = faceKeypoints.part(20).y() - height - 10;

    /* Calculate rotation angle */
    cv::Point pt1(faceKeypoints.part(31).x(),faceKeypoints.part(31).y());
    cv::Point pt2(faceKeypoints.part(35).x(),faceKeypoints.part(35).y());
    double rotAngle = ImageUtils::calculateAngle(pt1,pt2);

    /* Rotate hat */
    hat_rotated = ImageUtils::rotateImage(m_hat_img,rotAngle);

    /* Resize hat */
    cv::Rect fRect(x0,y0,width,height);
    cv::resize(hat_rotated,hat_resized,cv::Size(fRect.width,fRect.height));

    /* Draw hat */
    result = ImageUtils::drawPngImage(frame,hat_resized,x0,y0);

    return result;
}

cv::Mat FaceFancyMan::processGlasses(cv::Mat &frame, dlib::full_object_detection &faceKeypoints)
{
    cv::Mat glasses_rotated,glasses_resized, result;

    /* Calculate width and height for glasses rectangle */

    int width    = (faceKeypoints.part(45).x() - faceKeypoints.part(36).x())+40;
    int height   = (faceKeypoints.part(28).y() - faceKeypoints.part(37).y())+40;
    int x0       = faceKeypoints.part(36).x()  - 20;
    int y0       = faceKeypoints.part(28).y()  - 25;

    /* Calculate rotation angle */
    cv::Point pt1(faceKeypoints.part(31).x(),faceKeypoints.part(31).y());
    cv::Point pt2(faceKeypoints.part(35).x(),faceKeypoints.part(35).y());

    /* Rotate glasses */
    double rotAngle = ImageUtils::calculateAngle(pt1,pt2);
    glasses_rotated = ImageUtils::rotateImage(m_glasses_img,rotAngle);

    /* Resize glasses to fit */
    cv::Rect fRect(x0,y0,width,height);
    cv::resize(glasses_rotated,glasses_resized,cv::Size(fRect.width,fRect.height));

    /* Draw glasses over frame */
    result = ImageUtils::drawPngImage(frame,glasses_resized,x0,y0);

    return result;
}

cv::Mat FaceFancyMan::processMustaches(cv::Mat &frame, dlib::full_object_detection &faceKeypoints)
{
    cv::Mat mustaches_rotated,mustaches_resized, result;

    /* Calculate width and height for mustaches rectangle */

    int width    = (faceKeypoints.part(35).x() - faceKeypoints.part(32).x())+45;
    int height   = (faceKeypoints.part(51).y() - faceKeypoints.part(30).y())+5;
    int x0       = faceKeypoints.part(31).x()  - 15;
    int y_offset = (faceKeypoints.part(51).y() - faceKeypoints.part(33).y())-5;
    int y0       = faceKeypoints.part(30).y()  + y_offset;

    /* Calculate rotation angle */
    cv::Point pt1(faceKeypoints.part(31).x(),faceKeypoints.part(31).y());
    cv::Point pt2(faceKeypoints.part(35).x(),faceKeypoints.part(35).y());

    /* Rotate mustaches */
    double rotAngle = ImageUtils::calculateAngle(pt1,pt2);
  mustaches_rotated = ImageUtils::rotateImage(m_mustaches_img,rotAngle);

    /* Resize mustaches to fit */
    cv::Rect fRect(x0,y0,width,height);
    cv::resize(mustaches_rotated,mustaches_resized,cv::Size(fRect.width,fRect.height));

    /* Draw mustaches over frame */
    result = ImageUtils::drawPngImage(frame,mustaches_resized,x0,y0);

    return result;
}
