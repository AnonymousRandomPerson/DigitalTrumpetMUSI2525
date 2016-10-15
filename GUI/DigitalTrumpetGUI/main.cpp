#include "mainwindow.h"
#include <QApplication>

#include "../../C++/TrumpetGenerator.h"

int main(int argc, char *argv[])
{
    TrumpetGenerator *trumpet = new TrumpetGenerator();
    trumpet->generateTrumpet(440, 2);

    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
