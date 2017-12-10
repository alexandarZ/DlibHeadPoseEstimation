#include "opengl_view.h"


opengl_view::opengl_view(QWidget *parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    cam.rotX=0.0;
    cam.rotY=0.0;
    cam.rotZ=0.0;
    cam.posZ=-4.5;

    create_texture = true;
    this->setParent(parent);
}

void opengl_view::update_texture(cv::Mat img)
{
    cv::Mat tx_img;

    //Flip image
    cv::flip(img,tx_img,0);

    //Get bytes from image
    int img_size = tx_img.total() * tx_img.elemSize();

    GLsizei width  = tx_img.cols;
    GLsizei height = tx_img.rows;

    byte* img_bytes = (byte*) malloc(img_size*sizeof(byte));
    std::memcpy(img_bytes,tx_img.data,img_size * sizeof(byte));

    //Create texture only first time
    if(create_texture)
    {
        glGenTextures(1, &frame_texture);
        glBindTexture(GL_TEXTURE_2D,frame_texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, img_bytes);

        create_texture = false;
    }
    else
    {
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_BGR,GL_UNSIGNED_BYTE,img_bytes);
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glActiveTexture(GL_TEXTURE0);

    glGenerateMipmap(GL_TEXTURE_2D);

    free(img_bytes);

    updateGL();
}

void opengl_view::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0,0,0,1.0);

    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
}

void opengl_view::resizeGL(int width, int height)
{
    glViewport(0,0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = GLfloat(width) / height;
    glFrustum(-x, x, -x, x, 3, 50);
    glMatrixMode(GL_MODELVIEW);
}

void opengl_view::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, cam.posZ);
    glRotatef(cam.rotX, 1.0, 0.0, 0.0);
    glRotatef(cam.rotY, 0.0, 1.0, 0.0);
    glRotatef(cam.rotZ, 0.0, 0.0, 1.0);

    drawFrame();
}

void opengl_view::mousePressEvent(QMouseEvent *event)
{
    //cam.lastPos = event->pos();
}

void opengl_view::mouseMoveEvent(QMouseEvent *event)
{
    //    GLfloat dx = GLfloat(event->x() - cam.lastPos.x()) / width();
    //    GLfloat dy = GLfloat(event->y() - cam.lastPos.y()) / height();

    //    if(event->buttons() & Qt::LeftButton) {
    //        cam.rotX += 180 * dy;
    //        cam.rotY += 180 * dx;
    //        updateGL();
    //    } else if(event->buttons() & Qt::RightButton) {
    //        cam.rotX += 180 * dy;
    //        cam.rotZ += 180 * dx;
    //        updateGL();
    //    }

    //    cam.lastPos = event->pos();
}

void opengl_view::wheelEvent(QWheelEvent *event)
{
    //    cam.posZ+=((float)event->delta())*0.01;
    //    updateGL();
    //    event->accept();
}

void opengl_view::drawFrame()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame_texture);

    glColor3f(1,1,1);

    glBegin(GL_QUADS);

    float x = 2.0;
    float y = 2.0;

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( x, -y, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( x, y, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, y, 0.0f);

    glEnd();
}
