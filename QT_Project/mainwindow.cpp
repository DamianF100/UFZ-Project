#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>
#include <QTimer>

using namespace std;
mt19937 rangen(43289739);
uniform_int_distribution<int> randist_1_27(1,27);

// Definitions

const int HEIGHT       = 500;
const int WIDTH        = 500;
const int SIZE_COLONIE = 200;

int option = 1;

double BREED_TRESH = 24;
int    NEIG_TRESH  = 28;
int    NEIG_DIST   = 8;
int    NEIG_EFF    = 15;

int    BREEDING[HEIGHT][WIDTH];
double BREEDING_INDEX[HEIGHT][WIDTH];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Plot->SetYcells(HEIGHT);
    ui->Plot->SetXcells(WIDTH);

    ui->Plot->LegendAdd(0, "White");
    ui->Plot->LegendAdd(1, "Red");
    ui->Plot->LegendAdd(2, "Green");

    running = false;

    connect(&timer, &QTimer::timeout, this, &MainWindow::on_STEP_clicked);
    timer.start(50);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_INIT_clicked()
{
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            BREEDING[y][x] = 0;

            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                BREEDING_INDEX[y][x] = randist_1_27(rangen);
            }
            }

        }

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                if(BREEDING_INDEX[y][x] < BREED_TRESH){
                    BREEDING[y][x] = 1;
                }
                else{BREEDING[y][x] = 2;}
            }

        }
    }

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            ui->Plot->SetXY(y, x, BREEDING[y][x]);

        }
    }

    ui->Plot->replot();
}

void MainWindow::on_STEP_clicked()

{
    if(sender() == &timer && !running) return;

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){

                int COUNT_NEIG = 0;
                int y_start    = y - NEIG_DIST;
                int x_start    = x - NEIG_DIST;

                for(int y_n = y_start; y_n < y_start + NEIG_DIST * 2; y_n++){
                    for(int x_n = x_start; x_n < x_start + NEIG_DIST * 2; x_n++){

                        if(BREEDING[y_n][x_n] == 2){

                            COUNT_NEIG ++;

                        }

                    }
                }
                if(option == 1){

                    if(COUNT_NEIG > NEIG_TRESH && BREEDING_INDEX[y][x] >  12 && BREEDING[y][x] != 2){
                        BREEDING_INDEX[y][x] = 24;

                    }
                    else{
                        BREEDING_INDEX[y][x]++;
                    }
                }
                else{
                         if (BREEDING_INDEX[y][x] < 24 - NEIG_EFF){
                        BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] + NEIG_EFF;
                      }
                     else if(BREEDING_INDEX[y][x] >24 - NEIG_EFF && BREEDING_INDEX[y][x] < 24){
                             BREEDING_INDEX[y][x] = 24;
                      }
                     else{
                         BREEDING_INDEX[y][x] ++;
                    }
                      BREEDING_INDEX[y][x] ++;
                }



                if(BREEDING_INDEX[y][x] > 27){
                    BREEDING_INDEX[y][x] = 1;
                }

                if(BREEDING_INDEX[y][x] < BREED_TRESH){
                    BREEDING[y][x] = 1;
                }
                else{BREEDING[y][x] = 2;}
            }
        }
    }

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            ui->Plot->SetXY(y, x, BREEDING[y][x]);

        }
    }

    ui->Plot->replot();
}


void MainWindow::on_RUN_clicked()
{
    running = !running;
}

