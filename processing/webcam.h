#ifndef WEBCAM_H
#define WEBCAM_H

#include "iprocessingitem.h"


class Webcam : public IProcessingItem
{
public:
    Webcam();
    ~Webcam();

    int  initialize(int cam=0);
    void process(ProcessingResult result);

    int  start();
    void stop();

private:
    cv::VideoCapture m_webcam;
    int m_webcam_id;
};

#endif // WEBCAM_H
