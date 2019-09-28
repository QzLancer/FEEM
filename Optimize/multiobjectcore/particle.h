#include <QStringList>

class Particle
{
public:
	//构造函数
    Particle(int _numberOfVariables,
        int _numberOfObjectives,
        const double *_lowerBounds,
        const double *_upperBounds,
        const double *_vmax,
        double _c1, double _c2,
        const double *_weight,
        const double *_vari,
        void(*_functionPtr)(Particle *),
        QStringList _optimizeMode);
	//赋值运算符重载
	Particle& operator =(const Particle& str);
	//复制构造函数
	Particle(const Particle& str);

	//析构函数
	~Particle();
	
	//粒子初始化
	void inicializeParticle();
	//粒子位置更新
	void updateParticle(const double*);
	//粒子最佳值更新
	
	//设定粒子值
	void setValue(const double *_value);
	//设定约束满足程度
	void setConstraits(double _constraits);
	//设定可行性
	void setFeasible(bool _feasible);
	//设定被占优次数
	void setParetoDominance(int _paretoDominance);
	//返回被占优次数
	int getParetoDominance();
	//返回粒子位置
	const double *getPosition();
	//返回粒子速度
	//返回粒子值
	const double *getValue();



	//返回变量个数
	int getNumberOfVariables();
	//返回目标个数
	int getNumberOfObjective();
		
	//返回粒子最佳位置
	const double *getBestPosition();
	//返回粒子最佳速度
	//返回粒子最佳值
	const double *getBestValue();
	
	//返回可行性
	bool getFeasible();
	bool getBestFeasible();
	

	//输出Print
	void printParticle();
	void printParticleBest();

private:
	//变量个数
	int numberOfVariables;
	//优化目标个数
	int numberOfObjectives;

	//粒子位置
	double *position;
	//粒子速度
	double *velocity;
	//粒子值
	double *value;
	//粒子最佳位置
	double *bestPosition;
	//粒子最佳值
	double *bestValue;
	//粒子可行性
	bool feasible;
	bool bestFeasible;


	//权重
	static const double *weight;
	static const double *vari;
	static double c1, c2;
	//约束（在优化程序中进行设定）
	double constraits;
	//粒子被占优次数
	int paretoDominance;


	//粒子位置上界
	static const double *upperBounds;
	//粒子位置下界
	static const double *lowerBounds;
	//粒子最大速度
	static const double *maxVelocity;

	//计算粒子值
	static void(*functionPtr) (Particle *);
	//更新最佳粒子
	void updateParticleBest();

    QStringList optimizeMode;
};


