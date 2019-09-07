#ifndef BOUNCECAL_H
#define BOUNCECAL_H

/*
 * 理论编程计算直动式继电器动态特性、碰撞弹跳主程序
 * By QzLancer
 * 2019/9/6
 */

#include <QWidget>
#include <QString>
#include <QList>

class QCustomPlot;
class BounceCore : public QObject
{
	Q_OBJECT

public:
    BounceCore();
    BounceCore(QString fileName);
	~BounceCore();
	void Init(QString fileName);
	void bounceCalculate(double t0, double tn);
    void plot(QCustomPlot *widget);

private:
	QList<QList<double>> readExcel(QString fileName);
	double fxh(double xx, double vx, double xd, double vd, double Ef);
	double fdh(double xx, double vx, double xd, double vd);
	double impact(double k, double cm, double d, double e, double q, double q0, double v);
	double step(double x, double x0, double h0, double x1, double h1);
	QList<QList<double>> Ef_t; //从excel中读取的矩阵
	double dt;  //定义ODE45程序每一步的计算步长/s
	double D_kj;    //开距/m
	double D_xc;    //行程/m
	double K_c;     //超程弹簧刚度N/m
	double K_f;     //返回弹簧刚度N/m
	double M_d;     //动触头质量/kg
	double M_x;     //衔铁与连杆的质量/kg
	double K_p;     //碰撞刚度N/m
	double D_p;     //碰撞深度/m
	double Da_p;    //碰撞阻尼系数
	double f10;     //返回弹簧预压力/N
	double f20;     //超程弹簧预压力/N
	double e;       //碰撞指数
    QVector<double> t;
    QVector<double> xd;
    QVector<double> xx;
};

#endif // BOUNCECAL_H
