#ifndef PROCESSINGRESULT_H
#define PROCESSINGRESULT_H

#include "dlib/opencv.h"
#include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/gui_widgets.h"

#include <QDebug>

class ProcessingResult
{
public:
    ProcessingResult();

    cv::Mat camera_frame;
    std::vector<dlib::full_object_detection> face_shapes;
    std::vector<cv::Vec3d> face_euler_angles;
    std::vector<dlib::rectangle> face_rectangles;
    std::vector<std::vector<cv::Vec6f>> face_wireframe_models;
};

#endif // PROCESSINGRESULT_H
