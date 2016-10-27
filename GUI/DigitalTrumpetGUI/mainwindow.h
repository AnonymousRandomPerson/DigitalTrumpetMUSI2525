#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QString>

#include <array>

#include "audioplayer.h"

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
    void setFrequency(int frequency);
    void setValve1(int frequency);
    void setValve2(int frequency);
    void setValve3(int frequency);

private:
    Ui::MainWindow *ui;
    QLabel *vibrationLabel;
    AudioPlayer *audioPlayer;
    std::array<bool, 3> valves = {false, false, false};
    double frequency;

    const QString FREQUENCY_TEXT = "Frequency: ";

    void playSound();
};

#endif // MAINWINDOW_H
