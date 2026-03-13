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
int window = 100;

//Effect options
bool OPTION_NEIG = FALSE;
bool OPTION_SEAS = FALSE;


// Definitions of the size

const int HEIGHT       = 100; //Height of the the entire simulated area
const int WIDTH        = 100; //Width of the entire simulated ares
// we have 2826 coupleees
const int SIZE_COLONIE = 30;  //Radius of the simulted colonie - It is a square with this line size


// Paremeter for the neighbour relation
double BREED_TRESH = 24; //Threshold after which a individuum starts breeding


// interesting parameter combinations: (24,40,10,10)
// interesting parameter combinations: (24,30,8,5)
//Arrays to simulate
int    BREEDING[HEIGHT][WIDTH]; //Either 0 if an individuum is not breeding - 1 if it is
double BREEDING_INDEX[HEIGHT][WIDTH]; //Index how high an individuum is in the cycle - 1 t0 27
int    NEIGHBOURS[HEIGHT][WIDTH];

// Parameters for sensitivity analysis
int PARAMETERS[10*3][6];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   //Initialises the plot
    ui->setupUi(this);
    ui->COLLONY->SetYcells(HEIGHT);
    ui->COLLONY->SetXcells(WIDTH);

    //Plot colors
    ui->COLLONY->LegendAdd(0, RGB(255, 140, 0));
    ui->COLLONY->LegendAdd(1, RGB(51, 115, 184));
    ui->COLLONY->LegendAdd(2, "White");


    // Strong Summer (Deep, saturated orange)
    ui->COLLONY->LegendAdd(3,  RGB(255, 140, 0));   // January - Summer (Strong)
    ui->COLLONY->LegendAdd(4,  RGB(255, 150, 30));  // February - Summer (Strong)

    // Autumn (Strong, slightly shifting to a standard orange)
    ui->COLLONY->LegendAdd(5,  RGB(255, 165, 0));   // March - Autumn start (Strong)
    ui->COLLONY->LegendAdd(6,  RGB(255, 180, 60));  // April - Autumn (Medium-Strong)

    // Transition / Winter (Fainting - adding more white/light)
    ui->COLLONY->LegendAdd(7,  RGB(255, 200, 120)); // May - Late autumn (Fading)
    ui->COLLONY->LegendAdd(8,  RGB(255, 220, 170)); // June - Winter (Faint)
    ui->COLLONY->LegendAdd(9,  RGB(255, 235, 200)); // July - Deep winter (Faintest / Pale Peach)
    ui->COLLONY->LegendAdd(10, RGB(255, 220, 170)); // August - Late winter (Faint)

    // Spring (Strengthening back toward Summer)
    ui->COLLONY->LegendAdd(11, RGB(255, 200, 100)); // September - Early spring (Gaining strength)
    ui->COLLONY->LegendAdd(12, RGB(255, 180, 50));  // October - Spring (Medium-Strong)
    ui->COLLONY->LegendAdd(13, RGB(255, 160, 20));  // November - Late spring (Strong)
    ui->COLLONY->LegendAdd(14, RGB(255, 140, 0));   // December - Summer start (Strongest)


    running = false;
    connect(&timer, &QTimer::timeout, this, & MainWindow::on_STEP_clicked);
    timer.start(SPEED);

    ui->Timeseries->addGraph();
    ui->Timeseries->xAxis->setLabel(" ");
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

    int ROW_COUNT = 0;
    for(int col = 0; col < 6; col++){
        for (int row = 0; row < 10*3; row++) {

            if(ROW_COUNT == 10){
                ROW_COUNT = 0;
            }

            // Breeding threshold
            if(col == 0){
                if(ROW_COUNT == 0){
                    PARAMETERS[row][col] = 1;
                }
                else{PARAMETERS[row][col] = 10*ROW_COUNT;
                }
            }

            // Interaction
            if(col == 1){
                if(ROW_COUNT == 0){
                    PARAMETERS[row][col] = 3;
                }
                else{PARAMETERS[row][col] = 3*ROW_COUNT;
                }
            }

            // Effect
            if(col == 2){
                if(ROW_COUNT == 0){
                    PARAMETERS[row][col] = 1;
                }
                else{PARAMETERS[row][col] = ROW_COUNT+1;
                }
            }
            // Rest
            if(col == 3){
                if(ROW_COUNT == 0){
                    PARAMETERS[row][col] = 2;
                }
                else{PARAMETERS[row][col] = 2*(ROW_COUNT+1);
                }
            }

            // Season threshold
            if(col == 4){
                if(row < 10){
                    PARAMETERS[row][col] = 0;
                }
                else{PARAMETERS[row][col] = 1;
                }
            }

            // Neighbours threshold
            if(col == 5){
                if(row > 9){
                    PARAMETERS[row][col] = 0;
                }
                else{PARAMETERS[row][col] = 1;
                }
            }
            ROW_COUNT ++;
        }
    }

    // for (int row = 0; row < 10*3; row++) {
    //     cout << PARAMETERS[row][0] << endl;
    // }
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
            if( (y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE ){ //Going over the array

                BREEDING_INDEX[y][x] ++; //all get one more

                if (BREEDING_INDEX[y][x] <= BREED_TRESH){
                    if(OPTION_NEIG){
                        if (NEIGHBOURS[y][x] > NEIG_TRESH && BREEDING_INDEX[y][x] > REST ){
                                BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] + NEIG_EFF;
                        }
                    }

                    if(OPTION_SEAS){
                        if(MONTH == 1)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x];      // JAN - normal
                        if(MONTH == 2)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x];      // FEB - normal
                        if(MONTH == 3)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x]-1;  // MAR - worse time
                        if(MONTH == 4)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x]-1;  // APR - worse time
                        if(MONTH == 5)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x]-1;  // MAY - worse time
                        if(MONTH == 6)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x];      // JUN - normal
                        if(MONTH == 7)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x];      // JUL - normal
                        if(MONTH == 8)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x];      // AUG - normal
                        if(MONTH == 9)  BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x]+1;  // SEP - good time
                        if(MONTH == 10) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x] +1;  // OCT - good time
                        if(MONTH == 11) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x]+1 ;  // NOV - good time
                        if(MONTH == 12) BREEDING_INDEX[y][x] = BREEDING_INDEX[y][x];      // DEC - normal
                    }
                }

                //If the Index is higher than the Breed_threshold because of the effects from above please set it to 24 such that they are not faster in their breeding time
                if( BREEDING_INDEX[y][x] > BREED_TRESH &&  BREEDING[y][x] ==1 ) {
                    BREEDING_INDEX[y][x] = BREED_TRESH +1;

                }



                // Updates the breeding grid
                if(BREEDING_INDEX[y][x] > 27){
                    BREEDING_INDEX[y][x] = 1;
                }

                if(BREEDING_INDEX[y][x] <= BREED_TRESH){

                    BREEDING[y][x] = 1;
                }
                else{BREEDING[y][x] = 2;}
            }

            if(OPTION_SEAS){ //Color Background
                if( !((y - HEIGHT/2)*(y - HEIGHT/2) + (x - WIDTH/2)*(x - WIDTH/2) <= SIZE_COLONIE*SIZE_COLONIE) ){
                    BREEDING[y][x] = MONTH +2;
                }
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

    //Set out here
    // outfile << timestep << "," << NEIG_TRESH << "," << NEIG_DIST  << "," << NEIG_EFF   << ","<< REST << "," << sum_breeding << std::endl;

    time_data.push_back(timestep);
    breeding_data.push_back(sum_breeding);
    timestep++;

    ui->Timeseries->graph(0)->setData(time_data, breeding_data);

    QStringList monthNames = {"Jan","","","Apr","","","Jul","","","Okt","",""};
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    for(int i = 0; i <= timestep; i++){
        textTicker->addTick(i, monthNames[i % 12]);
    }
    ui->Timeseries->xAxis->setTicker(textTicker);

    int visibleSteps = 60;
    if(timestep > visibleSteps)
        ui->Timeseries->xAxis->setRange(timestep - visibleSteps, timestep);
    else
        ui->Timeseries->xAxis->setRange(0, visibleSteps);

    ui->Timeseries->replot();

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

