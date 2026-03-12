#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>
#include <QTimer>
#include <iostream>

using namespace std;

//Radom generater
mt19937 rangen(42);
uniform_int_distribution<int> randist_1_27(1,27);


int speed_timer = 30;

//Effect options
bool OPTION_NEIG = FALSE;
bool OPTION_SEAS = FALSE;


// Definitions of the size

const int HEIGHT       = 100; //Height of the the entire simulated area
const int WIDTH        = 100; //Width of the entire simulated ares
const int SIZE_COLONIE = 30;  //Radius of the simulted colonie - It is a square with this line size


// Paremeter for the neighbour relation
double BREED_TRESH = 24; //Threshold after which a individuum starts breeding


// interesting parameter combinations: (24,40,10,10)
// interesting parameter combinations: (24,30,8,5)
//Arrays to simulate
int    BREEDING[HEIGHT][WIDTH]; //Either 0 if an individuum is not breeding - 1 if it is
double BREEDING_INDEX[HEIGHT][WIDTH]; //Index how high an individuum is in the cycle - 1 t0 27
int    NEIGHBOURS[HEIGHT][WIDTH];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   //Initialises the plot
    ui->setupUi(this);
    ui->COLLONY->SetYcells(HEIGHT);
    ui->COLLONY->SetXcells(WIDTH);

    //Plot colors
    ui->COLLONY->LegendAdd(0, "White");
    ui->COLLONY->LegendAdd(1, "Red");
    ui->COLLONY->LegendAdd(2, "Green");

    running = false;
    connect(&timer, &QTimer::timeout, this, & MainWindow::on_STEP_clicked);
    timer.start(SPEED);

    ui->Timeseries->addGraph();
    ui->Timeseries->xAxis->setLabel("Time step");
    ui->Timeseries->yAxis->setLabel("Breeding individuals");

    ui->Timeseries->xAxis->setRange(0,100);
    ui->Timeseries->yAxis->setRange(0, SIZE_COLONIE*SIZE_COLONIE * 3.14 );
    
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Options

void MainWindow::on_PRINT_TRESH_cursorPositionChanged(int arg1, int arg2)
{
    NEIG_TRESH = ui->PRINT_TRESH->text().toDouble();
}

void MainWindow::on_PRINT_DIST_cursorPositionChanged(int arg1, int arg2)
{
    NEIG_DIST = ui->PRINT_DIST->text().toInt();
}

void MainWindow::on_PRINT_EFF_cursorPositionChanged(int arg1, int arg2)
{
    NEIG_EFF = ui->PRINT_EFF->text().toInt();
}

void MainWindow::on_PRINT_SPEED_cursorPositionChanged(int arg1, int arg2)
{
    SPEED = ui->PRINT_SPEED->text().toInt();
}

void MainWindow::on_PRINT_REST_cursorPositionChanged(int arg1, int arg2)
{
    REST = ui->PRINT_REST->text().toInt();
}


void MainWindow::on_CURSOR_SPEED_sliderMoved(int position)
{
    SPEED = position;
    ui->PRINT_SPEED->setText(QString::number(position));

    timer.stop();
    timer.start(SPEED);
}

void MainWindow::on_CURSOR_REST_sliderMoved(int position)
{
    REST = position;
    ui->PRINT_REST->setText(QString::number(position));
}

void MainWindow::on_CURSOR_TRESH_sliderMoved(int position)
{
    NEIG_TRESH = position;
    ui->PRINT_TRESH->setText(QString::number(position));
}

void MainWindow::on_CURSOR_DIST_sliderMoved(int position)
{
    NEIG_DIST = position;
    ui->PRINT_DIST->setText(QString::number(position));
}

void MainWindow::on_CURSOR_EFF_sliderMoved(int position)
{
    NEIG_EFF = position;
    ui->PRINT_EFF->setText(QString::number(position));
}


void MainWindow::on_OPTION_SEAS_clicked()
{
    OPTION_SEAS = !OPTION_SEAS;
    ui->OPTION_SEAS->setText(OPTION_SEAS ? "ON" : "OFF");
}

void MainWindow::on_OPTION_NEIG_clicked()
{
    OPTION_NEIG = !OPTION_NEIG;
    ui->OPTION_NEIG->setText(OPTION_NEIG ? "ON" : "OFF");
}

//Initialise the arrays
void MainWindow::on_INIT_clicked()
{   time_data.clear();
    breeding_data.clear();
    timestep = 0;

    ui->Timeseries->graph(0)->setData(time_data, breeding_data);
    ui->Timeseries->replot();
    //Set the Breeding_index to a normal random distribution
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            BREEDING[y][x]   = 0;
            NEIGHBOURS[y][x] = 0;

            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                BREEDING_INDEX[y][x] = randist_1_27(rangen);
            }
        }
    }

    //Update the Breeding array to the correct value which it is set from the random distribution
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

    //Give the plot
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            ui->COLLONY->SetXY(y, x, BREEDING[y][x]);
        }
    }
    ui->COLLONY->replot();

}

//One time step
void MainWindow::on_STEP_clicked(){

    if(sender() == &timer && !running) return;


    ui->MONTH->setText(MONTHS[MONTH-1]);

    // Count neighbours
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){

                if(OPTION_NEIG){
                    int COUNT_NEIG = 0; //counts the neighbour which are breeding
                    int y_start    = y - NEIG_DIST;
                    int x_start    = x - NEIG_DIST;
                    for(int y_n = y_start; y_n < y_start + NEIG_DIST * 2; y_n++){
                        for(int x_n = x_start; x_n < x_start + NEIG_DIST * 2; x_n++){
                            if(BREEDING[y_n][x_n] == 2){
                                COUNT_NEIG ++;
                            }
                        }
                    }
                NEIGHBOURS[y][x] = COUNT_NEIG;
                }
            }

        }
    }

    // Update breeding status and index

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
                if(OPTION_NEIG){
                    if (NEIGHBOURS[y][x] > NEIG_TRESH && BREEDING_INDEX[y][x] > REST && BREEDING_INDEX[y][x] < BREED_TRESH ){
                            BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] + NEIG_EFF;
                    }
                }

                if(OPTION_SEAS){
                    if(MONTH == 12 || MONTH == 1 || MONTH == 2) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] +2; // SUMMER - NORMAL UPDATE
                    if(MONTH == 3 || MONTH == 4 || MONTH == 5) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] -4; //AUTUMN - BREED_INDEX STAYS THE SAME, NO GOOD TIME FOR THE THING
                    if(MONTH == 6 || MONTH == 7 || MONTH == 8) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] -2; //WINTER - NORMAL BREEDING UPDATE, SPRING COMES, THEY STARTING TO THE MOOD
                    if(MONTH == 9 || MONTH == 10 || MONTH == 11) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] +4; //SPRING - FASTER UPDATE, THEY ARE IN THE MOOD
                }

                //If the Index is higher than the Breed_threshold because of the effects from above please set it to 24 such that they are not faster in their breeding time
                if( BREEDING_INDEX[y][x] > BREED_TRESH &&  BREEDING[y][x] ==1 ) {
                    BREEDING_INDEX[y][x] = BREED_TRESH - 1;
                }



                BREEDING_INDEX[y][x] ++;



                // Updates the breeding grid
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

    MONTH ++;
    if (MONTH == 13) MONTH =1;


    int sum_breeding = 0;

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            if(BREEDING[y][x] == 2){
                sum_breeding++;
            }
        }
    }

    time_data.push_back(timestep);
    breeding_data.push_back(sum_breeding);
    timestep++;

    ui->Timeseries->graph(0)->setData(time_data, breeding_data);
    ui->Timeseries->xAxis->setRange(0, timestep);
    ui->Timeseries->replot();

    // Shows the plot
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){

            ui->COLLONY->SetXY(y, x, BREEDING[y][x]);
        }
    }
    ui->COLLONY->replot();
}


void MainWindow::on_RUN_clicked()
{
    running = !running;
}

void MainWindow::on_MONTH_textEdited(const QString &arg1)
{
}

