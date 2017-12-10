#include "webcam.h"

Webcam::Webcam()
{

}

Webcam::~Webcam()
{

}

int Webcam::initialize(int cam)
{
    this->m_webcam_id = cam;
    return 0;
}

void Webcam::process(ProcessingResult result)
{
    //Grab frame from webcam
    if(m_webcam.isOpened())
    {
        m_webcam.read(result.camera_frame);

        //Propagate frame to next object in chain
        IProcessingItem::process(result);
    }
}

int Webcam::start()
{
    this->m_webcam.open(m_webcam_id);

    if(this->m_webcam.isOpened())
    {
        return 0;
    }

    return 1;
}

void Webcam::stop()
{
    this->m_webcam.release();
}
