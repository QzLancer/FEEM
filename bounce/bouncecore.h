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
#include <QMap>

class QCustomPlot;
class BounceCore : public QObject
{
	Q_OBJECT

public:
    BounceCore();
    BounceCore(QString fileName);
	~BounceCore();
    void Init(QString fileName);
    void initMaterialProperties(double open_distance, double stroke, double mov_contact_mass, double armatrue_mass);
    void initSpringReactionForce(double overtravel_spring_stiffness, double return_spring_stiffness, double overtravel_spring_preload, double return_spring_preload);
    void initCollisionContact(double stiffness, double depth, double damping, double index);
    void initSolveProperties(double initial_time, double end_time, double step_size);
    void bounceCalculate();
    void plot(QCustomPlot *widget);
    QVector<double> gett() const;
    QVector<double> getxd() const;
    QVector<double> getxx() const;

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
    double t0;      //起始时间
    double tn;      //结束时间
    QVector<double> t;
    QVector<double> xd;
    QVector<double> xx;
};

#endif // BOUNCECAL_H
