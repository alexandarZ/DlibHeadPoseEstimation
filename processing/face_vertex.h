#ifndef FACEVERTEXGENERATOR_H
#define FACEVERTEXGENERATOR_H

#include "iprocessingitem.h"

class FaceVertexGenerator : public IProcessingItem
{
public:
    FaceVertexGenerator();
    ~FaceVertexGenerator();

    int initialize();
    void process(ProcessingResult result);

private:

};

#endif // FACEVERTEXGENERATOR_H
