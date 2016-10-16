#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSlider>
#include <QCheckBox>

#include "audioplayer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int windowLength = 300;
    int windowHeight = 200;

    setFixedSize(windowLength, windowHeight);

    int valveHeight = 30;
    int valveLength = 70;
    int valveY = 10;
    int valveMiddleX = (windowLength - valveLength) / 2;
    int valveSeparation = valveLength + 2;

    QCheckBox *valve1 = new QCheckBox("Valve 1", this);
    valve1->setGeometry(valveMiddleX - valveSeparation, valveY, valveLength, valveHeight);
    QObject::connect(valve1, SIGNAL(stateChanged(int)), this, SLOT(setValve1(int)));

    QCheckBox *valve2 = new QCheckBox("Valve 2", this);
    valve2->setGeometry(valveMiddleX, valveY, valveLength, valveHeight);
    QObject::connect(valve2, SIGNAL(stateChanged(int)), this, SLOT(setValve2(int)));

    QCheckBox *valve3 = new QCheckBox("Valve 3", this);
    valve3->setGeometry(valveMiddleX + valveSeparation, valveY, valveLength, valveHeight);
    QObject::connect(valve3, SIGNAL(stateChanged(int)), this, SLOT(setValve3(int)));

    int sliderHeight = 100;
    int sliderLength = 30;
    int sliderX = 10;
    int sliderY = (windowHeight - sliderHeight) / 2;
    int labelHeight = 30;

    QSlider *vibration = new QSlider(this);
    vibration->setGeometry(sliderX, sliderY, sliderLength, sliderHeight);
    vibration->setRange(0, 1000);
    vibrationLabel = new QLabel(FREQUENCY_TEXT + "0Hz", this);
    vibrationLabel->setGeometry(sliderX + sliderLength, sliderY + labelHeight, 120, labelHeight);
    QObject::connect(vibration, SIGNAL(valueChanged(int)), this, SLOT(setFrequency(int)));

    audioPlayer = new AudioPlayer();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete audioPlayer;
}

void MainWindow::setFrequency(int frequency)
{
    vibrationLabel->setText(FREQUENCY_TEXT + QString::number(frequency) + "Hz");
}

void MainWindow::setValve1(int state)
{
    audioPlayer->start();
}

void MainWindow::setValve2(int state)
{
    audioPlayer->start();
}

void MainWindow::setValve3(int state)
{
    audioPlayer->start();
}
