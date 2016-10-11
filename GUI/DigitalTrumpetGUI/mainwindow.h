#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QString>

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

private:
    Ui::MainWindow *ui;
    QLabel *vibrationLabel;

    const QString FREQUENCY_TEXT = "Frequency: ";
};

#endif // MAINWINDOW_H
