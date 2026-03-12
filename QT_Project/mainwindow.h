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

    void on_CURSOR_TRESH_sliderMoved(int position);

    void on_CURSOR_DIST_sliderMoved(int position);

    void on_CURSOR_EFF_sliderMoved(int position);

    void on_CURSOR_SPEED_sliderMoved(int position);

    void on_CURSOR_REST_sliderMoved(int position);


    void on_PRINT_TRESH_cursorPositionChanged(int arg1, int arg2);

    void on_PRINT_DIST_cursorPositionChanged(int arg1, int arg2);

    void on_PRINT_EFF_cursorPositionChanged(int arg1, int arg2);

    void on_PRINT_SPEED_cursorPositionChanged(int arg1, int arg2);

    void on_PRINT_REST_cursorPositionChanged(int arg1, int arg2);


private:
    Ui::MainWindow *ui;

    QTimer timer;
    bool running;

    int NEIG_TRESH = 1;
    int NEIG_DIST  = 1;
    int NEIG_EFF   = 1;
    int SPEED      = 1;
    int REST       = 1;

    QVector<double> time_data;
    QVector<double> breeding_data;
    int timestep = 0;

};
#endif // MAINWINDOW_H
