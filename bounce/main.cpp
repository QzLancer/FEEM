#include "bouncecore.h"
#include <QApplication>
#include <qcustomplot.h>
#include <QDebug>
#include "bouncewidget.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    BounceCore core;
    qDebug() << QDir::currentPath();
    core.Init("D:/FEEM/bounce/cosim3D_force.xlsx");
    core.bounceCalculate(0, 0.08);

//    BounceWidget bw;
//    bw.show();

    BounceTab bt;
    bt.addLine("test1");
    bt.addLine("test2");
    bt.show();




	return a.exec();
}
