#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    estimator = new HeadPoseEstimator(ui->openGLWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionStart_webcam_triggered()
{
    estimator->processWebcam(true);
}

void MainWindow::on_actionStop_webcam_triggered()
{
     estimator->processWebcam(false);
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionRender_head_angles_toggled(bool show)
{
    estimator->showHeadAngles(show);
}

void MainWindow::on_actionRender_face_points_toggled(bool show)
{
    estimator->showFacePoints(show);
}

void MainWindow::on_actionRender_face_vertex_model_toggled(bool show)
{
    estimator->showVertexModel(show);
}

void MainWindow::on_actionSnapchat_dog_triggered(bool show)
{
    estimator->showSnapchatDogOverlay(show);
}

void MainWindow::on_actionFancy_hat_triggered(bool show)
{
    estimator->showFancyManOverlay(show);
}

void MainWindow::on_actionLoad_source_face_image_triggered()
{
    bool estimatorStatus = estimator->isWorking();

    if(estimator->isWorking())
    {
        estimator->processWebcam(false);
    }

    QString imgFile = QFileDialog::getOpenFileName(this,("Open image with source face"),
            QApplication::applicationDirPath(),("PNG image files (*.png)"));

    if(!imgFile.isEmpty())
    {
        estimator->loadNewFace(imgFile);
    }

    estimator->processWebcam(estimatorStatus);
}

void MainWindow::on_actionStart_toggled(bool show)
{
  estimator->swapFace(show);
}
