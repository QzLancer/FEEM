#include "particle.h"
#include <QString>
using namespace std;

class PSO
{
public:
	//构造函数
    PSO(int _numberOfParticles,
        int _numberOfVariables,
        int _numberOfObjectives,
        int _numberOfExtraParticles,
        double *_lowerBounds,
        double *_upperBounds,
        double *_vmax,
        void(*_objectiveFunction)(Particle *),
        double _lowerWeight,
        double _upperWeight,
        int _maxIteration,
        double _c1,
        double _c2,
        double _vari,
        QStringList _optimizeMode,
        QString _stoppingCriteria,
        QString _psoType);
	//析构函数
	~PSO();

	//优化过程
	void optimize();
	//输出最佳值
	//const double *getBestValue();
	//输出最佳位置
	//const double *getBestPosition();
	//输出最佳值适应度
	bool getBestFeasibility();
	//打印最佳值
	void printBest();
	//距离计算
	double getnorm(const double *A, const double *V);

private:

	//初始化整个种群
	void initializeParticles();
	//更新整个种群
	void updateParticles();
	//更新全局最优
	void updateGlobalBest();
	//更新粒子占优情况
	void updateDomiance();
	//更新经营种群
	void updateExtraParticles();
	//更新？？好像没啥用
	void updateConvergence();
	//轮盘赌
	void RussianBest();
	//打印第n个粒子
	void printParticle(int);
	//整个粒子群
	Particle **particles;
	//精英粒子群
	Particle **extraParticles;
	//中间粒子
	//Particle *tempParticle;
	//粒子个数
	int numberOfParticles;
	//粒子精英种群粒子个数上限
	int numberOfExtraParticles;
	//粒子精英种群现存粒子数
	int sizeOfExtraParticles;
	//优化目标个数
	int numberOfObjectives;
	//变量个数
	int numberOfVariables;
	//最佳粒子位置
	int bestParticleIndex;
	//权重
	double weight;
	//变异概率
	double vari;
	//粒子位置上界
	double upperWeight;
	//粒子位置上界
	double lowerWeight;
	//最大迭代次数
	int maxIteration;
	//适应度
	double* distance;

	double threshold;

    QString psoType;

    QString stoppingCriteria;

	double impBest();

	double impAv();

	double noAce();

	double movObj();

	double movPor();

	double stdDev();

	double maxDist();

	double maxDistQuick();

	double comCrit();

	double diffMaxDistQuick();
	
};

