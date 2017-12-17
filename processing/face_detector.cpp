#include "face_detector.h"
#include <QApplication>

FaceDetector::FaceDetector()
{
    this->m_face_downsample_ratio=2.0;
    this->setEnabled(false);
}

FaceDetector::~FaceDetector()
{

}

int FaceDetector::initialize( float face_downsample_ratio)
{
    QString face_detector_path = QApplication::applicationDirPath()+"/shape_predictor_68_face_landmarks.dat";

    //Deserialize face shape descriptor
    try
    {
        dlib::deserialize(face_detector_path.toStdString()) >> m_pose_model;
    }
    catch(...)
    {
        qDebug()<<"Error - Could not load face shape descriptors!";
        return 1;
    }

    //Set face_downsample ratio
    this->m_face_downsample_ratio = face_downsample_ratio;

    //Initialize face detector
    m_face_detector = dlib::get_frontal_face_detector();

    return 0;
}

void FaceDetector::process(ProcessingResult result)
{
    cv::Mat im_small;

    // Resize image for face detection
    //result.camera_frame.copyTo(im_small);
    cv::resize(result.camera_frame, im_small, cv::Size(), 1.0/m_face_downsample_ratio, 1.0/m_face_downsample_ratio);

    // Change to dlib's image format. No memory is copied.
    dlib::cv_image<dlib::bgr_pixel> cimg_small(im_small);
    dlib::cv_image<dlib::bgr_pixel> cimg(result.camera_frame);

    // Detect faces on resize image
    m_faces = m_face_detector(cimg_small);

    // Find key points of each face.
    for (unsigned long i = 0; i < m_faces.size(); ++i)
    {
        // Resize obtained rectangle for full resolution image.
        dlib::rectangle r(
                    (long)(m_faces[i].left()   * m_face_downsample_ratio),
                    (long)(m_faces[i].top()    * m_face_downsample_ratio),
                    (long)(m_faces[i].right()  * m_face_downsample_ratio),
                    (long)(m_faces[i].bottom() * m_face_downsample_ratio)
                    );

        // Detect points
        dlib::full_object_detection shape = m_pose_model(cimg, r);

        //Render face points on each face
        if(this->m_enabled)
        {
            renderFacePoints(result.camera_frame,shape);
        }

        //Add shape into result
        result.face_shapes.push_back(shape);

        //Add face rectangle into result
        result.face_rectangles.push_back(r);
    }

    IProcessingItem::process(result);
}

void FaceDetector::renderFacePoints(cv::Mat &img, dlib::full_object_detection &face)
{
    for(int i=0;i<face.num_parts();i++)
    {
        cv::Point face_point(face.part(i).x(), face.part(i).y());
        cv::circle(img,face_point,2,cv::Scalar(0,255,0),-1);
    }
}
