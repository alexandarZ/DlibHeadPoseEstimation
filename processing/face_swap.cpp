#include "face_swap.h"
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
                result.camera_frame = swapFace(result.camera_frame,dstFacePoints);
            }
        }
        catch(...)
        {
            qDebug()<<"Could not swap face for some reason!";
        }
    }

    IProcessingItem::process(result);
}

cv::Mat FaceSwapGenerator::swapFace(cv::Mat dstFaceImage,std::vector<cv::Point> dstFacePoints)
{
    cv::Mat dstFaceWarped,output;

    // Convert cv::Mat to float data type
    dstFaceImage.convertTo(dstFaceWarped, CV_32F);
    dstFaceImage.convertTo(dstFaceImage, CV_32F);

    // Find convex hull of faces
    std::vector<cv::Point2f> dstFaceHull;
    std::vector<cv::Point2f> srcFaceHull;
    std::vector<int> hullIndex;

    cv::convexHull(dstFacePoints, hullIndex, false, false);

    for(int i = 0; i < hullIndex.size(); i++)
    {
        srcFaceHull.push_back(m_face_src_points[hullIndex[i]]);
        dstFaceHull.push_back(dstFacePoints[hullIndex[i]]);
    }

    qDebug()<<"FaceSwap - ConvexHull done!";

    // Find delaunay triangulation of destination face
    std::vector<std::vector<int>> dt;
    cv::Rect rect(0, 0, dstFaceWarped.cols, dstFaceWarped.rows);
    calculateDelaunayTriangles(rect, srcFaceHull, dt);

    qDebug()<<"FaceSwap - Delaunay done!";

    // Apply affine transformation to Delaunay triangles
    for(size_t i = 0; i < dt.size(); i++)
    {
        std::vector<cv::Point2f> t1, t2;
        // Get points for img1, img2 corresponding to the triangles
        for(size_t j = 0; j < 3; j++)
        {
            t1.push_back(dstFaceHull[dt[i][j]]);
            t2.push_back(srcFaceHull[dt[i][j]]);
        }

        warpAndBlend(dstFaceImage, dstFaceWarped, t1, t2);
    }

    qDebug()<<"FaceSwap - Affine transformation done!";

    // Calculate mask
    std::vector<cv::Point> hull8U;

    for(int i = 0; i < srcFaceHull.size(); i++)
    {
        cv::Point pt(srcFaceHull[i].x, srcFaceHull[i].y);
        hull8U.push_back(pt);
    }

    cv::Mat mask = cv::Mat::zeros(m_face_src_img.rows, m_face_src_img.cols, CV_8UC3);
    cv::fillConvexPoly(mask,&hull8U[0], hull8U.size(), cv::Scalar(255,255,255));

    qDebug()<<"FaceSwap - Mask done!";

    // Clone face
    cv::Rect r = cv::boundingRect(srcFaceHull);
    cv::Point center = (r.tl() + r.br()) / 2;

    dstFaceWarped.convertTo(dstFaceWarped, CV_8UC3);
    cv::seamlessClone(dstFaceWarped,m_face_src_img, mask, center, output, cv::NORMAL_CLONE);

    qDebug()<<"FaceSwap - Face clone done!";

    //Show images
    //cv::imshow("Src image",m_face_src_img);
    //cv::imshow("Original image",dstFaceImage);
    //cv::imshow("Mask",mask);

    return output;
}

int FaceSwapGenerator::openSrcFaceImage(QString faceSrcImage)
{
    //Load source face image
    cv::String img_path(faceSrcImage.toStdString());
    cv::Mat new_face = cv::imread(img_path,CV_LOAD_IMAGE_COLOR);

    if(!new_face.data)
    {
        qDebug()<<"Error - Could not load source face image!";
        return 1;
    }

    //Update face source image
    this->m_face_src_img = new_face;

    //Get face key points
    int result = readSrcFaceKeypoints();

    if(result > 0)
    {
        return result;
    }

    //Convert Mat to float data type
    m_face_src_img.convertTo(m_face_src_img, CV_32F);

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
        m_face_src_points.push_back(cv::Point2f(shape.part(i).x(),shape.part(i).y()));
    }

    qDebug()<<"FaceSwap successfully initialized srcImage points";
    return 0;
}

void FaceSwapGenerator::calculateDelaunayTriangles(cv::Rect rect, std::vector<cv::Point2f> &points, std::vector<std::vector<int> > &delaunayTri)
{
    // Create an instance of Subdiv2D
    cv::Subdiv2D subdiv(rect);

    // Insert points into subdiv
    for(std::vector<cv::Point2f>::iterator it = points.begin(); it != points.end(); it++)
        subdiv.insert(*it);

    std::vector<cv::Vec6f> triangleList;
    subdiv.getTriangleList(triangleList);
    std::vector<cv::Point2f> pt(3);
    std::vector<int> ind(3);

    for( size_t i = 0; i < triangleList.size(); i++ )
    {
        cv::Vec6f t = triangleList[i];
        pt[0] = cv::Point2f(t[0], t[1]);
        pt[1] = cv::Point2f(t[2], t[3]);
        pt[2] = cv::Point2f(t[4], t[5 ]);

        if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])){
            for(int j = 0; j < 3; j++)
                for(size_t k = 0; k < points.size(); k++)
                    if(abs(pt[j].x - points[k].x) < 1.0 && abs(pt[j].y - points[k].y) < 1)
                        ind[j] = k;

            delaunayTri.push_back(ind);
        }
    }
}

void FaceSwapGenerator::applyAffineTransform(cv::Mat &warpImage, cv::Mat &src, std::vector<cv::Point2f> &srcTri, std::vector<cv::Point2f> &dstTri)
{
    // Given a pair of triangles, find the affine transform.
    cv::Mat warpMat = cv::getAffineTransform( srcTri, dstTri );

    // Apply the Affine Transform just found to the src image
    cv::warpAffine( src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}

void FaceSwapGenerator::warpAndBlend(cv::Mat &img1, cv::Mat &img2, std::vector<cv::Point2f> &t1, std::vector<cv::Point2f> &t2)
{
    cv::Rect r1 = cv::boundingRect(t1);
    cv::Rect r2 = cv::boundingRect(t2);

    // Offset points by left top corner of the respective rectangles
    std::vector<cv::Point2f> t1Rect, t2Rect;
    std::vector<cv::Point> t2RectInt;

    for(int i = 0; i < 3; i++)
    {
        t1Rect.push_back( cv::Point2f( t1[i].x - r1.x, t1[i].y -  r1.y) );
        t2Rect.push_back( cv::Point2f( t2[i].x - r2.x, t2[i].y - r2.y) );
        t2RectInt.push_back( cv::Point(t2[i].x - r2.x, t2[i].y - r2.y) ); // for fillConvexPoly
    }

    // Get mask by filling triangle
    cv::Mat mask = cv::Mat::zeros(r2.height, r2.width, CV_32FC3);
    cv::fillConvexPoly(mask, t2RectInt, cv::Scalar(1.0, 1.0, 1.0), 16, 0);

    // Apply warpImage to small rectangular patches

    cv::Mat img1Rect(img1.size(),img1.type());

    img1(r1).copyTo(img1Rect);

    cv::Mat img2Rect = cv::Mat::zeros(r2.height, r2.width, img1Rect.type());

    applyAffineTransform(img2Rect, img1Rect, t1Rect, t2Rect);

    cv::multiply(img2Rect,mask, img2Rect);
    cv::multiply(img2(r2), cv::Scalar(1.0,1.0,1.0) - mask, img2(r2));
    img2(r2) = img2(r2) + img2Rect;
}

