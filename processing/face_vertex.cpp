#include "face_vertex.h"

FaceVertexGenerator::FaceVertexGenerator()
{
    this->setEnabled(false);
}

FaceVertexGenerator::~FaceVertexGenerator()
{

}

int FaceVertexGenerator::initialize()
{

}

void FaceVertexGenerator::process(ProcessingResult result)
{

    //Generate vertex model of each face
    for(int face_cnt=0;face_cnt<result.face_rectangles.size();face_cnt++)
    {
        try
        {
            // Create face rectangle
            cv::Rect face_rect(0,0,result.camera_frame.cols,result.camera_frame.rows);

            // Create an instance of Subdiv2D
            cv::Subdiv2D subdiv(face_rect);

            int face_parts = result.face_shapes[face_cnt].num_parts();

            // Insert face points into Subdiv2D which generate wireframe model
            for(int i=0;i<face_parts;i++)
            {
                subdiv.insert(cv::Point2f(result.face_shapes[face_cnt].part(i).x(),result.face_shapes[face_cnt].part(i).y()));
            }

            //Triangulate points using Delaunay triangulation
            std::vector<cv::Vec6f> triangleList;
            subdiv.getTriangleList(triangleList);

            //Add wireframe model to result
            result.face_wireframe_models.push_back(triangleList);

            // Draw wireframe model
            if(this->m_enabled)
            {
                std::vector<cv::Point> pt(3);

                for( size_t i = 0; i < triangleList.size(); i++ )
                {
                    cv::Vec6f t = triangleList[i];

                    pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
                    pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
                    pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

                    // Draw triangles
                    if ( face_rect.contains(pt[0]) && face_rect.contains(pt[1]) && face_rect.contains(pt[2]))
                    {
                            cv::line(result.camera_frame, pt[0], pt[1], cv::Scalar(255,255,255), 0.8, CV_AA, 0);
                            cv::line(result.camera_frame, pt[1], pt[2], cv::Scalar(255,255,255), 0.8, CV_AA, 0);
                            cv::line(result.camera_frame, pt[2], pt[0], cv::Scalar(255,255,255), 0.8, CV_AA, 0);
                    }
                }
            }
        }
        catch(...)
        {
            qDebug()<<"Could not create vertex model of face!";
        }
    }

    IProcessingItem::process(result);
}
