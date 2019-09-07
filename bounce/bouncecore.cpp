#include "bouncecore.h"
#include <math.h>
#include <QAxObject>
#include <QDebug>
#include "qcustomplot.h"
BounceCore::BounceCore()
{
}

BounceCore::BounceCore(QString fileName)
{
    Init(fileName);
}

BounceCore::~BounceCore()
{

}

/*!
 *  @brief 参数初始化
 */
void BounceCore::Init(QString fileName)
{
	Ef_t = readExcel(fileName);
	dt = 0.00001;
	D_kj = 0.0017;
	D_xc = 0.00268;
	K_c = 13000;
	K_f = 370;
	M_d = 0.007;
	M_x = 0.0129;
	K_p = 5.3E+8;
	D_p = 0.0001;
	Da_p = 1000;
	f10 = 6;
	f20 = 7;
	e = 1.5;
}

/*!
 *  @brief 计算弹跳位移
 *  @param t0 : 仿真开始时间
 *  @param t1 : 仿真结束时间
 */
void BounceCore::bounceCalculate(double t0, double tn)
{
	QList<double>Ef = Ef_t[1];  //电磁力向量
	const unsigned int n = static_cast<unsigned int>((tn - t0) / dt);

    t.resize(n);
    xd.resize(n);
    xx.resize(n);
	double *vd = new double[n];
	double *vx = new double[n];
	for (int i = 0; i < n; ++i) {
        t[i] = 0;
        xd[i] = 0;
        xx[i] = 0;
		vd[i] = 0;
		vx[i] = 0;
	}

	double K1, K2, K3, K4;
	double L1, L2, L3, L4;
	double M1, M2, M3, M4;
	double N1, N2, N3, N4;

	for (int i = 0; i < n - 1; ++i) {
		t[i + 1] = t[i] + dt;

		K1 = fxh(xx[i], vx[i], xd[i], vd[i], Ef[i]) / M_x; //fxh表示计算衔铁（连杆）所受合力，K1表示衔铁的加速度
		L1 = vx[i];  //vx表示衔铁的速度
		M1 = fdh(xx[i], vx[i], xd[i], vd[i]) / M_d; //fdh表示计算动触头所受合力，M1表示动触头加速度
		N1 = vd[i]; //vd表示动触头的速度

		K2 = fxh(xx[i] + dt / 2 * L1, vx[i] + dt / 2 * K1, xd[i] + dt / 2 * N1, vd[i] + dt / 2 * M1, Ef[i]) / M_x;
		L2 = vx[i] + dt / 2 * K1;
		M2 = fdh(xx[i] + dt / 2 * L1, vx[i] + dt / 2 * K1, xd[i] + dt / 2 * N1, vd[i] + dt / 2 * M1) / M_d;
		N2 = vd[i] + dt / 2 * M1;;

		K3 = fxh(xx[i] + dt / 2 * L2, vx[i] + dt / 2 * K2, xd[i] + dt / 2 * N2, vd[i] + dt / 2 * M2, Ef[i]) / M_x;
		L3 = vx[i] + dt / 2 * K2;
		M3 = fdh(xx[i] + dt / 2 * L2, vx[i] + dt / 2 * K2, xd[i] + dt / 2 * N2, vd[i] + dt / 2 * M2) / M_d;
		N3 = vd[i] + dt / 2 * M2;

		K4 = fxh(xx[i] + dt * L3, vx[i] + dt * K3, xd[i] + dt * N3, vd[i] + dt * M3, Ef[i]) / M_x;
		L4 = vx[i] + dt * K3;
		M4 = fdh(xx[i] + dt * L3, vx[i] + dt * K3, xd[i] + dt * N3, vd[i] + dt * M3) / M_d;
		N4 = vd[i] + dt * M3;

		//qDebug() << "K1-4 : " << K1 << K2 << K3 << K4;
		//qDebug() << "L1-4 : " << L1 << L2 << L3 << L4;
		//qDebug() << "M1-4 : " << M1 << M2 << M3 << M4;
		//qDebug() << "N1-4 : " << N1 << N2 << N3 << N4;
		vx[i + 1] = vx[i] + dt / 6 * (K1 + 2 * K2 + 2 * K3 + K4);
		xx[i + 1] = xx[i] + dt / 6 * (L1 + 2 * L2 + 2 * L3 + L4);
		vd[i + 1] = vd[i] + dt / 6 * (M1 + 2 * M2 + 2 * M3 + M4);
		xd[i + 1] = xd[i] + dt / 6 * (N1 + 2 * N2 + 2 * N3 + N4);
//        qDebug() << i + 1 << xd[i + 1] << xx[i + 1];
	}

}

void BounceCore::plot(QCustomPlot *widget)
{
    widget->addGraph();
    widget->graph(0)->setData(t,xd);
    widget->graph(0)->rescaleAxes();

    widget->addGraph();
    widget->graph(1)->setData(t,xx);
    widget->graph(1)->setPen(QPen(Qt::red));
    widget->graph(1)->rescaleAxes(true);

    widget->replot();
}

/*!
 *  @brief 读取excel中的矩阵
 */
QList<QList<double>> BounceCore::readExcel(QString fileName)
{

	QAxObject *excel = NULL;
	QAxObject *workbooks = NULL;
	QAxObject *workbook = NULL;
	excel = new QAxObject("Excel.Application");
	if (!excel)
	{
		qDebug() << "EXCEL object lost!";
	}

	excel->dynamicCall("SetVisible(bool)", false);
	workbooks = excel->querySubObject("WorkBooks");
	workbook = workbooks->querySubObject("Open(QString, QVariant)", fileName);
	QAxObject * worksheet = workbook->querySubObject("WorkSheets(int)", 1);//打开第一个sheet
	QAxObject * usedrange = worksheet->querySubObject("UsedRange");//获取该sheet的使用范围对象
//    QAxObject * rows = usedrange->querySubObject("Rows");
//    QAxObject * columns = usedrange->querySubObject("Columns");
//    int intRows = rows->property("Count").toInt();
//    int intCols = columns->property("Count").toInt();
//    qDebug() << "xls rows : "<<intRows;
//    qDebug() << "xls cols : "<<intCols;

	QVariant var = usedrange->dynamicCall("Value");

	QVariantList varRows = var.toList();
	const int rowCount = varRows.size();
	qDebug() << "rowCount = " << rowCount;
	QVariantList rowData;
	rowData = varRows[0].toList();
	const int colCount = rowData.size();
	qDebug() << "colCount = " << colCount;

	QList<QList<double>> data;

	for (int i = 0; i < colCount; ++i) {
		QList<double> data0;
		for (int j = 0; j < rowCount; ++j) {
			rowData = varRows[j].toList();
			data0.push_back(rowData[i].toDouble());
		}
		data.push_back(data0);
	}

	delete usedrange;
	delete worksheet;
	delete workbook;
	delete workbooks;
	delete excel;

	return data;
}


/*!
	@brief xh表示计算衔铁（连杆）所受合力，模型中连杆和轭铁是螺栓固定在一起的，可以看成一个整体，也就是一个质量
 */
double BounceCore::fxh(double xx, double vx, double xd, double vd, double Ef)
{
	double C_f = K_f / 600 * 0.0436;    //返回弹簧阻尼系数
	double f1 = f10 + K_f * xx + vx * C_f;  //返回弹簧所受合力

	double C_c = K_c / 30000 * 1.852;   //超程弹簧阻尼系数
	double f2 = f20 + K_c * (xx - xd) + (vx - vd)*C_c;    //超程弹簧所受合力

	double ft = impact(K_p, Da_p, D_p, e, xx, 0, -vx);    //连杆和轭铁间的接触力N/m

	double fe = impact(K_p, Da_p, D_p, e, D_xc - xx, 0, vx);    //衔铁和轭铁间的碰撞力N/m

	double fk = impact(K_p, Da_p, D_p, e, xx - xd, 0, -vx + vd);  //连杆和动触头间的接触力N/m

	return Ef + ft + fk - fe - f1 - f2;   //衔铁（连杆）所受合力
}

/*!
	@brief 计算动触头所受合力
 */
double BounceCore::fdh(double xx, double vx, double xd, double vd)
{
	double C_c = K_c / 30000 * 1.852;    //超程弹簧阻尼系数
	double f2 = f20 + K_c * (xx - xd) + (vx - vd)*C_c;    //超程弹簧所受合力
	double fk = impact(K_p, Da_p, D_p, e, xx - xd, 0, -vx + vd);  //动触头与连杆之间的接触力
	double fj = impact(K_p, Da_p, D_p, e, D_kj - xd, 0, vd);    //动触头与静触头的碰撞力
	return f2 - fk - fj;    //动触头所受合力
}

double BounceCore::impact(double k, double cm, double d, double e, double q, double q0, double v)
{
	double f;
	if (q > q0) {
		f = 0;
	}
	else {
		f = k * pow((q0 - q), e) + cm * v*step(q, q0 - d, 1, q0, 0);
	}
	if (f < 0) {
		f = 0;
	}
	return f;
}

double BounceCore::step(double x, double x0, double h0, double x1, double h1)
{
	double a = h1 - h0;
	double d = (x - x0) / (x1 - x0);
	if (x <= x0) {
		return h0;
	}
	else if (x > x0 && x < x1) {
		return h0 + a * d*d*(3 - 2 * d);
	}
	return h1;
}
