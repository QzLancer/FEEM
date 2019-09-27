#include <QApplication>
#include "singleobjectdialog.h"
#include "multiobjectdialog.h"
#include <QString>
#include <singleobjectcore/pso.h>
//void objectiveFunction (Particle *Particle);
#include "inputparamwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//        单目标优化dialog测试
        SingleObjectDialog w;
        QStringList InputList;
        QStringList TargetList;
        InputList << "Input1" << "Input2" << "Input3";
        TargetList << "Target1" << "Target2" << "Target3" << "Target4";
        w.setInputList(InputList);
        w.setTargetList(TargetList);
        w.show();

    //    //单目标优化core测试
    //    double lower[2] = { -2, -2 };//Maxmum of imput parameters
    //    double upper[2] = { 2, 2 };//Minimum of imput parameters
    //    double vmax[2] = { 0.1, 0.1 };//粒子最大速度
    //    int numberOfParticles = 50;//Population size
    //    int numberOfVariables = 2;//输入变量个数
    //    int maxIteration = 500;//Time of PSO circles;
    //    double lowerWeight = 0.4;//W lower bound
    //    double upperWeight = 0.9;//W upper bound
    //    double c1 = 2, c2 = 2;//c1、c2
    //    QString stoppingCriteria = "none";
    //    QString psoType = "Classic";
    //    double threshold = 0.0001;
    //    clock_t begin = clock();
    //    double timeElapsed;

    //    PSO pso(numberOfParticles, numberOfVariables, lower, upper, vmax, objectiveFunction, lowerWeight, upperWeight, maxIteration, c1, c2, threshold, stoppingCriteria, psoType);

    //    pso.optimize();

    //    pso.printBest();

    //多目标优化dialog测试
    MultiObjectDialog md;
//    QStringList InputList, TargetList;
//    InputList << "Input1" << "Input2" << "Input3";
//    TargetList << "Target1" << "Target2" << "Target3" << "Target4";
    md.setInputList(InputList);
    md.setTargetList(TargetList);
    md.show();

    return a.exec();
}

//function to be minimized
//void objectiveFunction (Particle *Particle)
//{
//    const double *_position = Particle->getPosition ();
//    double _constraits;

//    if (Particle::getNumberOfVariables () != 2) {
//        throw "Number of variables in Particles does not match number of variables in objective function" ;
//    }

//    Particle->setValue(100 * pow((_position[1] - pow(_position[0], 2)), 2) + pow((1 - _position[0]), 2));
//    _constraits = _position[1] + _position[0] > 1 ? 10000000 * (_position[1] + _position[0] - 1) : 0;
//    //_constraits = 0;
//    Particle->setConstraits(_constraits);
//    Particle->setFeasible(_constraits ? false : true);
//    return;
//}
