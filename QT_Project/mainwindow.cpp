#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>
#include <iostream>

using namespace std;

// Random generator
mt19937 rangen(42);
uniform_int_distribution<int> randist_1_27(1,27);

const int HEIGHT       = 100;
const int WIDTH        = 100;
const int SIZE_COLONIE = 30;

double BREED_TRESH = 24;

int BREEDING[HEIGHT][WIDTH];
double BREEDING_INDEX[HEIGHT][WIDTH];
int NEIGHBOURS[HEIGHT][WIDTH];

bool OPTION_NEIG = false;
bool OPTION_SEAS = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->COLLONY->SetYcells(HEIGHT);
    ui->COLLONY->SetXcells(WIDTH);

    // Setup plot legend/colors
    ui->COLLONY->LegendAdd(0, RGB(255, 140, 0));
    ui->COLLONY->LegendAdd(1, RGB(51, 115, 184));
    ui->COLLONY->LegendAdd(2, "White");

    for(int i=3;i<=14;i++) ui->COLLONY->LegendAdd(i, RGB(255,140,0));

    running = false;
    connect(&timer, &QTimer::timeout, this, &MainWindow::on_STEP_clicked);
    timer.start(SPEED);

    ui->Timeseries->addGraph();
    ui->Timeseries->xAxis->setLabel(" ");
    ui->Timeseries->yAxis->setLabel("Breeding individuals");
    ui->Timeseries->xAxis->setRange(0,100);
    ui->Timeseries->yAxis->setRange(0, SIZE_COLONIE*SIZE_COLONIE*3.14);

    // Optionally start sensitivity automatically
    runSensitivityAnalysis();
}

MainWindow::~MainWindow()
{
    if(outfile.is_open()) outfile.close();
    delete ui;
}

// ----------------- Sliders & options -----------------

void MainWindow::on_CURSOR_TRESH_sliderMoved(int position) { NEIG_TRESH = position; ui->PRINT_TRESH->setText(QString::number(position)); timer.stop(); timer.start(SPEED);}
void MainWindow::on_CURSOR_DIST_sliderMoved(int position)   { NEIG_DIST = position;  ui->PRINT_DIST->setText(QString::number(position)); }
void MainWindow::on_CURSOR_EFF_sliderMoved(int position)    { NEIG_EFF = position;   ui->PRINT_EFF->setText(QString::number(position)); }
void MainWindow::on_CURSOR_SPEED_sliderMoved(int position)  { SPEED = position; ui->PRINT_SPEED->setText(QString::number(position)); timer.stop(); timer.start(SPEED);}
void MainWindow::on_CURSOR_REST_sliderMoved(int position)   { REST = position; ui->PRINT_REST->setText(QString::number(position)); }

void MainWindow::on_PRINT_TRESH_cursorPositionChanged(int,int) { NEIG_TRESH = ui->PRINT_TRESH->text().toInt(); }
void MainWindow::on_PRINT_DIST_cursorPositionChanged(int,int)  { NEIG_DIST = ui->PRINT_DIST->text().toInt(); }
void MainWindow::on_PRINT_EFF_cursorPositionChanged(int,int)   { NEIG_EFF = ui->PRINT_EFF->text().toInt(); }
void MainWindow::on_PRINT_SPEED_cursorPositionChanged(int,int) { SPEED = ui->PRINT_SPEED->text().toInt(); }
void MainWindow::on_PRINT_REST_cursorPositionChanged(int,int)  { REST = ui->PRINT_REST->text().toInt(); }

void MainWindow::on_OPTION_SEAS_clicked() { OPTION_SEAS = !OPTION_SEAS; ui->OPTION_SEAS->setText(OPTION_SEAS?"ON":"OFF"); }
void MainWindow::on_OPTION_NEIG_clicked() { OPTION_NEIG = !OPTION_NEIG; ui->OPTION_NEIG->setText(OPTION_NEIG?"ON":"OFF"); }

void MainWindow::on_RUN_clicked() { running = !running; }
void MainWindow::on_MONTH_textChanged(const QString&) { }

// ----------------- Initialization -----------------

void MainWindow::on_INIT_clicked()
{
    time_data.clear();
    breeding_data.clear();
    timestep = 0;
    MONTH = 1;

    ui->Timeseries->graph(0)->setData(time_data, breeding_data);
    ui->Timeseries->replot();

    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            BREEDING[y][x]=0;
            NEIGHBOURS[y][x]=0;
            if( (y-HEIGHT/2)*(y-HEIGHT/2) + (x-WIDTH/2)*(x-WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE )
                BREEDING_INDEX[y][x] = randist_1_27(rangen);
        }
    }

    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            if( (y-HEIGHT/2)*(y-HEIGHT/2) + (x-WIDTH/2)*(x-WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                BREEDING[y][x] = (BREEDING_INDEX[y][x]<BREED_TRESH)?1:2;
            }
        }
    }

    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            ui->COLLONY->SetXY(y,x,BREEDING[y][x]);
        }
    }
    ui->COLLONY->replot();
}
// ----------------- Single timestep -----------------

void MainWindow::on_STEP_clicked()
{ OPTION_SEAS = false;
  OPTION_NEIG = false;
    REST = 10;
  NEIG_EFF = 4;


    if(sender()==&timer && !running) return;

    ui->MONTH->setText(MONTHS[MONTH-1]);

    // Count neighbours
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            if( (y-HEIGHT/2)*(y-HEIGHT/2) + (x-WIDTH/2)*(x-WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                if(OPTION_NEIG){
                    int COUNT_NEIG = 0;
                    int y_start = y-NEIG_DIST;
                    int x_start = x-NEIG_DIST;
                    for(int y_n=y_start; y_n<y_start+NEIG_DIST*2; y_n++){
                        for(int x_n=x_start; x_n<x_start+NEIG_DIST*2; x_n++){
                            if(BREEDING[y_n][x_n]==2) COUNT_NEIG++;
                        }
                    }
                    NEIGHBOURS[y][x]=COUNT_NEIG;
                }
            }
        }
    }

    // Update breeding index & state
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            if( (y-HEIGHT/2)*(y-HEIGHT/2) + (x-WIDTH/2)*(x-WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                BREEDING_INDEX[y][x]++;
                if(BREEDING_INDEX[y][x]<=BREED_TRESH){
                    if(OPTION_NEIG && NEIGHBOURS[y][x]>NEIG_TRESH && BREEDING_INDEX[y][x]>REST)
                        BREEDING_INDEX[y][x]+=NEIG_EFF;

                    if(OPTION_SEAS){
                        if(MONTH>=3 && MONTH<=5) BREEDING_INDEX[y][x]-=1;
                        if(MONTH>=9 && MONTH<=11) BREEDING_INDEX[y][x]+=1;
                    }
                }

                if(BREEDING_INDEX[y][x]>BREED_TRESH && BREEDING[y][x]==1)
                    BREEDING_INDEX[y][x]=BREED_TRESH+1;

                if(BREEDING_INDEX[y][x]>27) BREEDING_INDEX[y][x]=1;
                BREEDING[y][x] = (BREEDING_INDEX[y][x]<=BREED_TRESH)?1:2;
            }
        }
    }

    MONTH++;
    if(MONTH==13) MONTH=1;

    int sum_breeding = 0;
    for(int y=0;y<HEIGHT;y++)
        for(int x=0;x<WIDTH;x++)
            if(BREEDING[y][x]==2) sum_breeding++;

    time_data.push_back(timestep);
    breeding_data.push_back(sum_breeding);
    timestep++;

    // Only update GUI if running normally
    if(running){
        ui->Timeseries->graph(0)->setData(time_data,breeding_data);
        ui->Timeseries->replot();
        for(int y=0;y<HEIGHT;y++)
            for(int x=0;x<WIDTH;x++)
                ui->COLLONY->SetXY(y,x,BREEDING[y][x]);
        ui->COLLONY->replot();
    }
}

// ----------------- Sensitivity Analysis -----------------

void MainWindow::runSensitivityAnalysis()
{
    outfile.open("sensitivity_output.csv");
    outfile << "NEIG_TRESH,NEIG_DIST,timestep,sum_breeding\n";

    OPTION_NEIG = true;
    OPTION_SEAS = false;

    for(int tresh=1;tresh<=50;tresh+=5){
        for(int dist=1;dist<=30;dist+=5){
            NEIG_TRESH = tresh;
            NEIG_DIST = dist;

            on_INIT_clicked();

            for(int step=0;step<240;step++){
                on_STEP_clicked();

                int sum_breeding=0;
                for(int y=0;y<HEIGHT;y++)
                    for(int x=0;x<WIDTH;x++)
                        if(BREEDING[y][x]==2) sum_breeding++;

                outfile << tresh << "," << dist << "," << step << "," << sum_breeding << "\n";
            }
        }
    }

    outfile.close();
    std::cout << "Sensitivity analysis finished.\n";
}
