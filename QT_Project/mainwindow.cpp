#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <random>
#include <QTimer>

using namespace std;

//Radom generater
mt19937 rangen(43289739);
uniform_int_distribution<int> randist_1_27(1,27);

//Options for the visualisation
int speed_timer = 50;

// Definitions of the size
const int HEIGHT       = 50;  //Height of the the entire simulated area
const int WIDTH        = 50; //Width of the entire simulated ares
const int SIZE_COLONIE = 3;  //Radius of the simulted colonie - It is a square with this line size

// Paremeter for the neighbour relation
int rest_time = 18; //Which time after breeding the individuum cannot breed
int option = 1; //switch between the different neigbour relation - either they start direct breeding or the index increases faster
double BREED_TRESH = 24; //Threshold after which a individuum starts breeding
int    NEIG_TRESH  = 28; //After how many breeding neighbours an individuum starts breeding
int    NEIG_DIST   = 8; //the number of indivuum which are considered for the counter - the # is (NEIG_DIST*2)^2
int    NEIG_EFF    = 15; //How much the index is increased if the threshold is activated


//Arrays to simulate
int    BREEDING[HEIGHT][WIDTH]; //Either 0 if an individuum is not breeding - 1 if it is
double BREEDING_INDEX[HEIGHT][WIDTH]; //Index how high an individuum is in the cycle - 1 t0 27

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   //Initialises the plot
    ui->setupUi(this);
    ui->Plot->SetYcells(HEIGHT);
    ui->Plot->SetXcells(WIDTH);

    //Plot colors
    ui->Plot->LegendAdd(0, "White");
    ui->Plot->LegendAdd(1, "Red");
    ui->Plot->LegendAdd(2, "Green");

    running = false;

    connect(&timer, &QTimer::timeout, this, &MainWindow::on_STEP_clicked);

    timer.start(speed_timer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Initialise the arrays
void MainWindow::on_INIT_clicked()
{
    //Set the Breeding_index to a normal random distribution
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            BREEDING[y][x] = 0;
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
            ui->Plot->SetXY(y, x, BREEDING[y][x]);
        }
    }
    ui->Plot->replot();
}

//One time step
void MainWindow::on_STEP_clicked(){

    //Automatic timestep update
    if(sender() == &timer && !running) return;

    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){
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
                if(option == 1){
                    if(COUNT_NEIG > NEIG_TRESH && BREEDING_INDEX[y][x] >  rest_time  && BREEDING[y][x] != 2){ //If the neighbours threshold is met, it is not in rest time and not breeding
                        BREEDING_INDEX[y][x] = BREED_TRESH; //set directly to breeding
                    }
                    else{
                        BREEDING_INDEX[y][x]++;
                    }
                }
                else{ //TODO be updated - check the logic
                         if (BREEDING_INDEX[y][x] < BREED_TRESH - NEIG_EFF){
                        BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] + NEIG_EFF;
                      }
                     else if(BREEDING_INDEX[y][x] >BREED_TRESH - NEIG_EFF && BREEDING_INDEX[y][x] < BREED_TRESH){
                             BREEDING_INDEX[y][x] = BREED_TRESH ;
                      }
                     else{
                         BREEDING_INDEX[y][x] ++;
                    }
                      BREEDING_INDEX[y][x] ++;
                }

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

    // Shows the plot
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

