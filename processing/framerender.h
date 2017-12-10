#ifndef FRAMERENDER_H
#define FRAMERENDER_H

#include "iprocessingitem.h"
#include "processingresult.h"
#include "opengl_view.h"

class FrameRender : public IProcessingItem
{
public:
    FrameRender();

    int initialize(opengl_view* gl_widget);
    void process(ProcessingResult result);

private:
    opengl_view* m_gl_widget;
};

#endif // FRAMERENDER_H
