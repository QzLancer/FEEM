#include "bouncecore.h"
#include <QApplication>
#include <qcustomplot.h>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QCustomPlot w;
    w.resize(800,600);
    w.show();

    BounceCore core;
    qDebug() << QDir::currentPath();
    core.Init("D:/FEEM/bounce/cosim3D_force.xlsx");
    core.bounceCalculate(0, 0.08);
    core.plot(&w);


	return a.exec();
}
