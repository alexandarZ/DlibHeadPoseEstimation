#include "head_posedetector.h"

HeadPoseDetector::HeadPoseDetector()
{
    this->setEnabled(false);
}

HeadPoseDetector::~HeadPoseDetector()
{

}

int HeadPoseDetector::initialize()
{
    clearData();
}

void HeadPoseDetector::process(ProcessingResult result)
{
    //Estimate head pose for each face
    foreach(dlib::full_object_detection face, result.face_shapes)
    {
        //2D model points
        std::vector<cv::Point2d> image_points;
        image_points.push_back( cv::Point2d(face.part(30).x() , face.part(30).y())  ); // Nose tip
        image_points.push_back( cv::Point2d(face.part(8).x()  , face.part(8).y())   ); // Chin
        image_points.push_back( cv::Point2d(face.part(36).x() , face.part(36).y()) );  // Left eye left corner
        image_points.push_back( cv::Point2d(face.part(45).x() , face.part(45).y()) );  // Right eye right corner
        image_points.push_back( cv::Point2d(face.part(48).x() , face.part(48).y()) );  // Left Mouth corner
        image_points.push_back( cv::Point2d(face.part(54).x() , face.part(54).y()) );  // Right mouth corner
        image_points.push_back( cv::Point2d(face.part(27).x() , face.part(27).y()) );

        // 3D model points
        std::vector<cv::Point3d> model_points;
        model_points.push_back(cv::Point3d(0.0f, 0.0f, 0.0f));          // Nose tip
        model_points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));     // Chin
        model_points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));  // Left eye left corner
        model_points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));   // Right eye right corner
        model_points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f)); // Left Mouth corner
        model_points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));  // Right mouth corner

        model_points.push_back(cv::Point3d(0.0f, 140.0f, 0.0f));

        // Camera internals
        double focal_length = result.camera_frame.cols;
        cv::Point2d center = cv::Point2d(result.camera_frame.cols/2,result.camera_frame.rows/2);
        cv::Mat camera_matrix = (cv::Mat_<double>(3,3) << focal_length, 0, center.x, 0 , focal_length, center.y, 0, 0, 1);
        cv::Mat dist_coeffs = cv::Mat::zeros(4,1,cv::DataType<double>::type); // Assuming no lens distortion

        cv::Mat rotation_vector; // Rotation in axis-angle form
        cv::Mat translation_vector;

        // Solve for pose
        cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, rotation_vector, translation_vector);

        // Project a 3D nose point onto the image plane.
        std::vector<cv::Point3d> nose_end_point3D;
        std::vector<cv::Point2d> nose_end_point2D;

        nose_end_point3D.push_back(cv::Point3d(0,0,150.0));

        cv::projectPoints(nose_end_point3D, rotation_vector, translation_vector, camera_matrix, dist_coeffs, nose_end_point2D);

        /* Convert calculated data into yaw, pitch and roll */
        cv::Mat rotCamerMatrix;
        cv::Rodrigues(rotation_vector,rotCamerMatrix);

        cv::Vec3d euler_angles;
        getEulerAngles(rotCamerMatrix,euler_angles);

        //Filter angle
        accumulator_pitch = (0.5 * euler_angles[0]) + (1.0 - 0.5) * accumulator_pitch;
        accumulator_yaw   = (0.5 * euler_angles[1]) + (1.0 - 0.5) * accumulator_yaw;
        accumulator_roll  = (0.5 * euler_angles[2]) + (1.0 - 0.5) * accumulator_roll;

        euler_angles[0] = accumulator_pitch;
        euler_angles[1] = accumulator_yaw;
        euler_angles[2] = accumulator_roll;

        //Put angles in result
        result.face_euler_angles.push_back(euler_angles);

        //Draw face angles
        if(this->m_enabled)
        {
            char yaw[40];
            char pitch[40];
            char roll[40];

            sprintf(pitch,"Rx: %d",accumulator_pitch);
            sprintf(yaw,"Ry: %d",accumulator_yaw);
            sprintf(roll,"Rz: %d",accumulator_roll);

            //Draw head angles
            renderHeadAngles(result.camera_frame,rotation_vector,translation_vector,camera_matrix);

            //Draw used points for head pose estimation
            //for(int i=0; i < image_points.size(); i++)
            //{
            //    cv::circle(result.camera_frame, image_points[i], 3, cv::Scalar(0,0,255), -1);
            //}

            //Draw nose line
            //cv::line(result.camera_frame,image_points[0], nose_end_point2D[0], cv::Scalar(255,0,0), 2);

            cv::putText(result.camera_frame,pitch,cv::Point(face.part(7).x(),face.part(7).y()+30),cv::FONT_HERSHEY_PLAIN,1.0,cv::Scalar(255,255,255));
            cv::putText(result.camera_frame,yaw,cv::Point(face.part(7).x(),face.part(7).y()+45),cv::FONT_HERSHEY_PLAIN,1.0,cv::Scalar(255,255,255));
            cv::putText(result.camera_frame,roll,cv::Point(face.part(7).x(),face.part(7).y()+60),cv::FONT_HERSHEY_PLAIN,1.0,cv::Scalar(255,255,255));
        }
    }

    IProcessingItem::process(result);
}

void HeadPoseDetector::clearData()
{
    accumulator_pitch = 0.0;
    accumulator_roll  = 0.0;
    accumulator_yaw   = 0.0;
}

void HeadPoseDetector::getEulerAngles(cv::Mat &cameraRotMatrix, cv::Vec3d &euler_angles)
{
    cv::Mat Rt;
    cv::transpose(cameraRotMatrix, Rt);
    cv::Mat shouldBeIdentity = Rt * cameraRotMatrix;
    cv::Mat I = cv::Mat::eye(3,3, shouldBeIdentity.type());
    float x, y, z;

    bool isSingularMatrix = (cv::norm(I, shouldBeIdentity) < 1e-6);

    if(!isSingularMatrix)
    {
        euler_angles[0] = 0;
        euler_angles[1] = 0;
        euler_angles[2] = 0;
        return;
    }

    float sy = sqrt(cameraRotMatrix.at<double>(0,0) * cameraRotMatrix.at<double>(0,0) +  cameraRotMatrix.at<double>(1,0) * cameraRotMatrix.at<double>(1,0) );

    bool singular = sy < 1e-6;

    if (!singular)
    {
        x = atan2(cameraRotMatrix.at<double>(2,1) , cameraRotMatrix.at<double>(2,2));
        y = atan2(-cameraRotMatrix.at<double>(2,0), sy);
        z = atan2(cameraRotMatrix.at<double>(1,0), cameraRotMatrix.at<double>(0,0));
    }
    else
    {
        x = atan2(-cameraRotMatrix.at<double>(1,2), cameraRotMatrix.at<double>(1,1));
        y = atan2(-cameraRotMatrix.at<double>(2,0), sy);
        z = 0;
    }

    x = x * 180.0f / M_PI;
    y = y * 180.0f / M_PI;
    z = z * 180.0f / M_PI;

    euler_angles[0]=-x;
    euler_angles[1]=y;
    euler_angles[2]=z;
}

void HeadPoseDetector::renderHeadAngles(cv::Mat &image, cv::Mat &rotationVector, cv::Mat &translationVector, cv::Mat &cameraMatrix)
{
    std::vector<cv::Point3f> axes;
    axes.push_back(cv::Point3f(0,0,0));
    axes.push_back(cv::Point3f(150.0,0,0));
    axes.push_back(cv::Point3f(0,150.0,0));
    axes.push_back(cv::Point3f(0,0,150.0));
    std::vector<cv::Point2f> projected_axes;

    cv::projectPoints(axes, rotationVector, translationVector, cameraMatrix, cv::noArray(), projected_axes);

    cv::line(image, projected_axes[0], projected_axes[3], cv::Scalar(255,0,0),2,CV_AA);
    cv::line(image, projected_axes[0], projected_axes[2], cv::Scalar(0,255,0),2,CV_AA);
    cv::line(image, projected_axes[0], projected_axes[1], cv::Scalar(0,0,255),2,CV_AA);
}
