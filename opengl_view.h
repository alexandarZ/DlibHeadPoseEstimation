#ifndef OPENGL_VIEW_H
#define OPENGL_VIEW_H

#include <QWidget>
#include <QMouseEvent>
#include <QGLWidget>
#include <QtOpenGL>
#include <QOpenGLFunctions>
#include <QVector>

#include <opencv2/core.hpp>

typedef struct
{
    GLfloat posX,posY,posZ;
    GLfloat rotX,rotY,rotZ;
    QPoint  lastPos;
}
Camera;

class opengl_view : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit opengl_view(QWidget *parent=0);

signals:

public slots:
    void update_texture(cv::Mat img);

protected:

    void initializeGL();
    void resizeGL(int width,int height);
    void paintGL();

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void drawFrame();
private:

     GLuint frame_texture;
     Camera cam;

     bool create_texture;

};

#endif // OPENGL_VIEW_H
