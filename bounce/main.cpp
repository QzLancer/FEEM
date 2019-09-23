#include "bouncecore.h"
#include <QApplication>
#include <qcustomplot.h>
#include <QDebug>
#include "bouncedialog.h"
#include <QTranslator>
#include <QWidget>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QTranslator translator;
    translator.load("zh_CN");
    a.installTranslator(&translator);

//    BounceCore core;
//    qDebug() << QDir::currentPath();
//    core.Init("D:/FEEM/bounce/cosim3D_force.xlsx");
//    core.bounceCalculate(0, 0.08);

//    QCustomPlot cp;
//    cp.show();
//    core.plot(&cp);

    BounceDialog bd;
    bd.show();

//    BounceTab bt;
//    bt.addLine("test1");
//    bt.addLine("test2");
//    bt.show();

//    QWidget *w = new QWidget;
//    w->show();


	return a.exec();
}
