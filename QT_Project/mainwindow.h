#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_INIT_clicked();

    void on_STEP_clicked();

    void on_RUN_clicked();

    void on_NEIG_TRESH_sliderMoved(int position);

    void on_NEI_DIST_sliderMoved(int position);

    void on_NEIG_EFF_sliderMoved(int position);


private:
    Ui::MainWindow *ui;

    QTimer timer;
    bool running;

    int NEIG_TRESH = 1;
    int NEIG_DIST  = 1;
    int NEIG_EFF   = 1;

    QVector<double> time_data;
    QVector<double> breeding_data;
    int timestep = 0;

};
#endif // MAINWINDOW_H
