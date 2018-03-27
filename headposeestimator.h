#ifndef HEADPOSEESTIMATOR_H
#define HEADPOSEESTIMATOR_H

#include <QObject>
#include <QTimer>

#include "processing/webcam.h"
#include "processing/framerender.h"
#include "processing/face_detector.h"
#include "processing/head_posedetector.h"
#include "processing/face_vertex.h"
#include "processing/face_snapchatdog.h"
#include "processing/face_fancyman.h"
#include "processing/face_swap.h"
#include "processing/face_rectangles.h"

class HeadPoseEstimator : public QObject
{
    Q_OBJECT
public:
    explicit HeadPoseEstimator(opengl_view* renderWidget,QObject *parent = nullptr);
    ~HeadPoseEstimator();

    void processWebcam(bool process);
    void showFacePoints(bool show);
    void showHeadAngles(bool show);
    void showVertexModel(bool show);
    void showFaceRectangles(bool show);

    //Overlays
    void showSnapchatDogOverlay(bool show);
    void showFancyManOverlay(bool show);

    //Face swap
    void loadNewFace(QString face_img_path);
    void swapFace(bool swap);

    //Status
    bool isWorking();
signals:

public slots:
    void run();

protected:

private:
    Webcam m_webcam;
    FrameRender m_renderer;
    FaceDetector m_face_detector;
    HeadPoseDetector m_head_pose_detector;
    FaceVertexGenerator m_vertex_generator;
    ProcessingResult m_result;
    FaceSnapchatDog m_snapchatdog_filter;
    FaceFancyMan m_fancyman_filter;
    FaceSwapGenerator m_faceswap_generator;
    FaceRectangles m_face_rectangles;

    bool m_running;
    QTimer m_loop_timer;
};

#endif // HEADPOSEESTIMATOR_H
