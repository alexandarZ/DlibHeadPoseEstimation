#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "headposeestimator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionStart_webcam_triggered();
    void on_actionStop_webcam_triggered();

    void on_actionExit_triggered();

    void on_actionRender_head_angles_toggled(bool show);

    void on_actionRender_face_points_toggled(bool show);

    void on_actionRender_face_vertex_model_toggled(bool show);

    void on_actionSnapchat_dog_triggered(bool show);

    void on_actionFancy_hat_triggered(bool show);

    void on_actionSwap_face_triggered(bool show);

private:
    Ui::MainWindow *ui;
    HeadPoseEstimator* estimator;
};

#endif // MAINWINDOW_H
