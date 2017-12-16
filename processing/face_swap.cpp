#include "face_swap.h"
#include "imageutils.h"
#include "QApplication"

FaceSwapGenerator::FaceSwapGenerator()
{
    this->setEnabled(false);

    //Deserialize face shape descriptor
    try
    {
        QString faceShapeDescriptorPath = QApplication::applicationDirPath()+"/shape_predictor_68_face_landmarks.dat";
        dlib::deserialize(faceShapeDescriptorPath.toStdString()) >> m_src_face_pose_model;
        this->m_src_face_detector = dlib::get_frontal_face_detector();
        qDebug()<<"FaceSwap initialized face predictor";
    }
    catch(...)
    {
        qDebug()<<"ERROR: FaceSwap could not load face shape descriptors!";
    }
}

FaceSwapGenerator::~FaceSwapGenerator()
{

}

int FaceSwapGenerator::initialize(QString faceSrcImage,int faceSrcDownsampleRatio)
{
    this->m_face_src_image_path       = faceSrcImage;
    this->m_face_src_downsample_ratio = faceSrcDownsampleRatio;

    //Open source face image
    int result = openSrcFaceImage(faceSrcImage);

    return result;
}

void FaceSwapGenerator::process(ProcessingResult result)
{
    if(this->m_enabled)
    {
        std::vector<cv::Point> dstFacePoints;

        // Process faces
        try
        {
            qDebug()<<"Getting face points";

            for(int i=0;i<result.face_rectangles.size();i++)
            {
                dstFacePoints.clear();

                //Get keypoints for face which will be swapped
                for(int j=0;j<result.face_shapes[i].num_parts();j++)
                {
                    dstFacePoints.push_back(cv::Point(result.face_shapes[i].part(j).x(), result.face_shapes[i].part(j).y()));
                }

                qDebug()<<"Swapping face";

                //Swap face
                result.camera_frame = swapFace(result.camera_frame,dstFacePoints,result.face_rectangles[i]);
            }
        }
        catch(...)
        {
            qDebug()<<"Could not swap face for some reason!";
        }
    }

    IProcessingItem::process(result);
}

cv::Mat FaceSwapGenerator::swapFace(cv::Mat& dstFaceImage, std::vector<cv::Point> dstFacePoints, dlib::rectangle dstFaceRect)
{
    cv::Mat srcMask,dstMask,srcWarppedMask,srcTransMatrix,srcFace,srcWarpedFace;

    //Create masks from source faces
    srcMask = getFaceMask(m_face_src_img,m_face_src_points);
    dstMask = getFaceMask(dstFaceImage,dstFacePoints);

    // Get transformation matrix between points of source and destination face
    srcTransMatrix = getAffineTransformationMatrix(m_face_src_points,dstFacePoints);

    // Rotate src image mask to match destination face mask
    cv::warpAffine(srcMask, srcWarppedMask, srcTransMatrix, dstFaceImage.size(), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));

    // Extract face from source image
    m_face_src_img.copyTo(srcFace, srcMask);

    // Warp face from source image to match destination face
    cv::Mat alignedSrcFace(dstFaceImage.size(), CV_8UC3, cv::Scalar::all(0));
    cv::warpAffine(srcFace, srcWarpedFace, srcTransMatrix, dstFaceImage.size(), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

    // Copy face from source image on destination face image
    srcWarpedFace.copyTo(alignedSrcFace, srcWarppedMask);

    // Color correct faces
    equalizeFaceColors(dstFaceImage,alignedSrcFace,srcWarppedMask);

    // Blur mask
    cv::Size featherAmount;
    int featherValue     = (int)cv::norm(dstFacePoints.at(0) - dstFacePoints.at(6)) / 12;
    featherAmount.width  = featherValue;
    featherAmount.height = featherValue;
    cv::erode(srcWarppedMask, srcWarppedMask, cv::getStructuringElement(cv::MORPH_RECT, featherAmount), cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::Scalar(0));
    cv::blur(srcWarppedMask, srcWarppedMask, featherAmount, cv::Point(-1, -1), cv::BORDER_REFLECT101);

    // Draw src. face over dst. face on frame
    for (size_t i = 0; i < dstFaceImage.rows; i++)
    {
        auto frame_pixel = dstFaceImage.row(i).data;
        auto faces_pixel = alignedSrcFace.row(i).data;
        auto masks_pixel = srcWarppedMask.row(i).data;

        for (size_t j = 0; j < dstFaceImage.cols; j++)
        {
            if (*masks_pixel != 0)
            {
                *frame_pixel = ((255 - *masks_pixel) * (*frame_pixel) + (*masks_pixel) * (*faces_pixel)) >> 8; // divide by 256
                *(frame_pixel + 1) = ((255 - *(masks_pixel + 1)) * (*(frame_pixel + 1)) + (*(masks_pixel + 1)) * (*(faces_pixel + 1))) >> 8;
                *(frame_pixel + 2) = ((255 - *(masks_pixel + 2)) * (*(frame_pixel + 2)) + (*(masks_pixel + 2)) * (*(faces_pixel + 2))) >> 8;
            }

            frame_pixel += 3;
            faces_pixel += 3;
            masks_pixel++;
        }
    }

    return dstFaceImage;
}

cv::Mat FaceSwapGenerator::getAffineTransformationMatrix(std::vector<cv::Point> srcFacePoints, std::vector<cv::Point> dstFacePoints)
{
    cv::Mat transformationMatrix;
    cv::Point2f affineTransKeyptsSrc[3],affineTransKeyptsDst[3];

    //Get affine transformation keypoints
    affineTransKeyptsSrc[0] = srcFacePoints.at(3);
    affineTransKeyptsSrc[1] = srcFacePoints.at(36);
    affineTransKeyptsSrc[2] = srcFacePoints.at(45);

    affineTransKeyptsDst[0] = dstFacePoints.at(3);
    affineTransKeyptsDst[1] = dstFacePoints.at(36);
    affineTransKeyptsDst[2] = dstFacePoints.at(45);

    //Get affine transformation matrix
    transformationMatrix = cv::getAffineTransform(affineTransKeyptsSrc, affineTransKeyptsDst);

    return transformationMatrix;
}

cv::Mat FaceSwapGenerator::getFaceMask(cv::Mat &face, std::vector<cv::Point> facePoints)
{
    cv::Mat mask;
    mask.create(face.size(), CV_8UC1);
    mask.setTo(cv::Scalar::all(0));

    // Find convex hull
    std::vector<int> hullPtsIndex;
    cv::convexHull(facePoints, hullPtsIndex, false, false);

    // Create convex polygon based on hull points
    cv::Point2i maskPoints[68];

    for(int i=0;i<hullPtsIndex.size();i++)
    {
        maskPoints[i] = facePoints[hullPtsIndex[i]];
    }

    // Fill mask polygon
    cv::fillConvexPoly(mask, maskPoints,hullPtsIndex.size(),cv::Scalar(255));

    return mask;
}

void FaceSwapGenerator::equalizeFaceColors(const cv::Mat source_image, cv::Mat target_image, cv::Mat& mask)
{
    uint8_t LUT[3][256];
    int source_hist_int[3][256];
    int target_hist_int[3][256];
    float source_histogram[3][256];
    float target_histogram[3][256];

    std::memset(source_hist_int, 0, sizeof(int) * 3 * 256);
    std::memset(target_hist_int, 0, sizeof(int) * 3 * 256);

    for (size_t i = 0; i < mask.rows; i++)
    {
        auto current_mask_pixel   = mask.row(i).data;
        auto current_source_pixel = source_image.row(i).data;
        auto current_target_pixel = target_image.row(i).data;

        for (size_t j = 0; j < mask.cols; j++)
        {
            if (*current_mask_pixel != 0) {
                source_hist_int[0][*current_source_pixel]++;
                source_hist_int[1][*(current_source_pixel + 1)]++;
                source_hist_int[2][*(current_source_pixel + 2)]++;

                target_hist_int[0][*current_target_pixel]++;
                target_hist_int[1][*(current_target_pixel + 1)]++;
                target_hist_int[2][*(current_target_pixel + 2)]++;
            }

            // Advance to next pixel
            current_source_pixel += 3;
            current_target_pixel += 3;
            current_mask_pixel++;
        }
    }

    // Calc CDF
    for (size_t i = 1; i < 256; i++)
    {
        source_hist_int[0][i] += source_hist_int[0][i - 1];
        source_hist_int[1][i] += source_hist_int[1][i - 1];
        source_hist_int[2][i] += source_hist_int[2][i - 1];

        target_hist_int[0][i] += target_hist_int[0][i - 1];
        target_hist_int[1][i] += target_hist_int[1][i - 1];
        target_hist_int[2][i] += target_hist_int[2][i - 1];
    }

    // Normalize CDF
    for (size_t i = 0; i < 256; i++)
    {
        source_histogram[0][i] = (source_hist_int[0][255] ? (float)source_hist_int[0][i] / source_hist_int[0][255] : 0);
        source_histogram[1][i] = (source_hist_int[1][255] ? (float)source_hist_int[1][i] / source_hist_int[1][255] : 0);
        source_histogram[2][i] = (source_hist_int[2][255] ? (float)source_hist_int[2][i] / source_hist_int[2][255] : 0);

        target_histogram[0][i] = (target_hist_int[0][255] ? (float)target_hist_int[0][i] / target_hist_int[0][255] : 0);
        target_histogram[1][i] = (target_hist_int[1][255] ? (float)target_hist_int[1][i] / target_hist_int[1][255] : 0);
        target_histogram[2][i] = (target_hist_int[2][255] ? (float)target_hist_int[2][i] / target_hist_int[2][255] : 0);
    }

    // Create lookup table

    auto binary_search = [&](const float needle, const float haystack[]) -> uint8_t
    {
        uint8_t l = 0, r = 255, m;
        while (l < r)
        {
            m = (l + r) / 2;
            if (needle > haystack[m])
                l = m + 1;
            else
                r = m - 1;
        }
        return m;
    };

    for (size_t i = 0; i < 256; i++)
    {
        LUT[0][i] = binary_search(target_histogram[0][i], source_histogram[0]);
        LUT[1][i] = binary_search(target_histogram[1][i], source_histogram[1]);
        LUT[2][i] = binary_search(target_histogram[2][i], source_histogram[2]);
    }

    // repaint pixels
    for (size_t i = 0; i < mask.rows; i++)
    {
        auto current_mask_pixel   = mask.row(i).data;
        auto current_target_pixel = target_image.row(i).data;

        for (size_t j = 0; j < mask.cols; j++)
        {
            if (*current_mask_pixel != 0)
            {
                *current_target_pixel = LUT[0][*current_target_pixel];
                *(current_target_pixel + 1) = LUT[1][*(current_target_pixel + 1)];
                *(current_target_pixel + 2) = LUT[2][*(current_target_pixel + 2)];
            }

            // Advance to next pixel
            current_target_pixel += 3;
            current_mask_pixel++;
        }
    }
}

int FaceSwapGenerator::openSrcFaceImage(QString faceSrcImage)
{
    //Load source face image
    cv::String img_path(faceSrcImage.toStdString());
    this->m_face_src_img = cv::imread(img_path,CV_LOAD_IMAGE_COLOR);

    if(!m_face_src_img.data)
    {
        qDebug()<<"Error - Could not load source face image!";
        return 1;
    }

    //Get face key points
    int result = readSrcFaceKeypoints();

    if(result > 0)
    {
        return result;
    }

    qDebug()<<"FaceSwap initialized image successfully!";

    //Return result
    return result;
}

int FaceSwapGenerator::readSrcFaceKeypoints()
{
    cv::Mat im_small;
    std::vector<dlib::rectangle> m_faces;

    // Resize image for face detection
    cv::resize(m_face_src_img, im_small, cv::Size(), 1.0/m_face_src_downsample_ratio, 1.0/m_face_src_downsample_ratio);

    // Change to dlib's image format. No memory is copied.
    dlib::cv_image<dlib::bgr_pixel> cimg_small(im_small);
    dlib::cv_image<dlib::bgr_pixel> cimg(m_face_src_img);

    // Detect faces on resize image
    m_faces = this->m_src_face_detector(cimg_small);

    // There is no faces?
    if(m_faces.size() == 0)
    {
        qDebug()<<"There are no faces on loaded image :(";
        return 1;
    }

    // Find key points for face.
    // Resize obtained rectangle for full resolution image.
    dlib::rectangle r(
                (long)(m_faces[0].left()   * m_face_src_downsample_ratio),
            (long)(m_faces[0].top()    * m_face_src_downsample_ratio),
            (long)(m_faces[0].right()  * m_face_src_downsample_ratio),
            (long)(m_faces[0].bottom() * m_face_src_downsample_ratio)
            );

    // Detect points
    dlib::full_object_detection shape = this->m_src_face_pose_model(cimg, r);

    // Add points into vector
    m_face_src_points.clear();

    for(uint32_t i=0;i<shape.num_parts();i++)
    {
        m_face_src_points.push_back(cv::Point2i(shape.part(i).x(),shape.part(i).y()));
    }

    qDebug()<<"FaceSwap successfully initialized srcImage points";
    return 0;
}
