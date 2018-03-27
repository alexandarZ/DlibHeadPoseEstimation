#ifndef FACERECTANGLES_H
#define FACERECTANGLES_H

#include "iprocessingitem.h"

class FaceRectangles : public IProcessingItem
{
public:
    FaceRectangles();
    ~FaceRectangles();

    int initialize();
    void process(ProcessingResult result);

};

#endif // FACERECTANGLES_H
