#include "framerender.h"

FrameRender::FrameRender()
{
}

int FrameRender::initialize(opengl_view *gl_widget)
{
    this->m_gl_widget = gl_widget;
    return 0;
}

void FrameRender::process(ProcessingResult result)
{
    //Draw frame
    m_gl_widget->update_texture(result.camera_frame);

    //Propagate
    IProcessingItem::process(result);
}
