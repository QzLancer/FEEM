#include "particle.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <exception>
#include <QObject>
#include <QDebug>

using namespace std;

//int Particle::numberOfVariables = 1;
//用的静态指针 要在外部定义
const double *Particle::lowerBounds = nullptr;

const double *Particle::upperBounds = nullptr;

const double *Particle::maxVelocity = nullptr;

double Particle::c1 = 2;

double Particle::c2 = 2;

const double *Particle::weight = nullptr;

const double *Particle::vari = nullptr;

void(*Particle::functionPtr) (Particle *);

Particle::Particle(int _numberOfVariables,
	int _numberOfObjectives,
	const double *_lowerBounds,
	const double *_upperBounds,
	const double *_vmax,
	double _c1, double _c2,
	const double *_weight, 
	const double *_vari,
    void(*_functionPtr)(Particle *),
    QStringList _optimizeMode)
{
    optimizeMode = _optimizeMode;

	lowerBounds = _lowerBounds;
	upperBounds = _upperBounds;

	numberOfVariables = _numberOfVariables;
	numberOfObjectives = _numberOfObjectives;

	if (numberOfVariables <= 0)
		throw "Number of variables can't be less or equal than zero.";
	if (numberOfObjectives <= 0)
		throw "Number of objectives can't be less or equal than zero.";

	c1 = _c1;
	c2 = _c2;
	weight = _weight;
	vari = _vari;
	functionPtr = _functionPtr;
	maxVelocity = _vmax;

    position = new double[static_cast<unsigned long long>(numberOfVariables)];
    velocity = new double[static_cast<unsigned long long>(numberOfVariables)];
    bestPosition = new double[static_cast<unsigned long long>(numberOfVariables)];
    value = new double[static_cast<unsigned long long>(numberOfObjectives)];
    bestValue = new double[static_cast<unsigned long long>(numberOfObjectives)];

	inicializeParticle();
}

Particle::Particle(const Particle& str)
{
	//变量个数
	numberOfVariables=str.numberOfVariables;
	//优化目标个数
	numberOfObjectives=str.numberOfObjectives;
	//粒子位置
    position = new double[static_cast<unsigned long long>(numberOfVariables)];
	//粒子速度
    velocity = new double[static_cast<unsigned long long>(numberOfVariables)];
	//粒子最佳位置
    bestPosition = new double[static_cast<unsigned long long>(numberOfVariables)];
	//位置速度复制
	for (int i = 0; i < numberOfVariables; i++) {
		position[i] = str.position[i];
		velocity[i] = str.velocity[i];
		bestPosition[i] = str.bestPosition[i];
	}
	//粒子值
    value = new double[static_cast<unsigned long long>(numberOfObjectives)];
	//粒子最佳值
    bestValue = new double[static_cast<unsigned long long>(numberOfObjectives)];
	for (int i = 0; i < numberOfObjectives; i++) {
		value[i] = str.value[i];
		bestValue[i] = str.bestValue[i];
	}
	//粒子可行性
	feasible = str.feasible;
	bestFeasible = str.bestFeasible;
	//权重
	c1 = str.c1;
	c2 = str.c2;
	weight = str.weight;
	vari = str.vari;
	functionPtr = str.functionPtr;
	maxVelocity = str.maxVelocity;
	//约束（在优化程序中进行设定）
	constraits = str.constraits;
	//粒子被占优次数
	paretoDominance = str.paretoDominance;
	//粒子位置上界下界
	lowerBounds = str.lowerBounds;
	upperBounds = str.upperBounds;
	//粒子最大速度
	maxVelocity = str.maxVelocity;
}

Particle& Particle::operator=(const Particle& str)
{
	if (this != &str)
	{
		//变量个数
		this->numberOfVariables = str.numberOfVariables;
		//优化目标个数
		this->numberOfObjectives = str.numberOfObjectives;
		//位置速度目标值复制
		for (int i = 0; i < numberOfVariables; i++) {
			this->position[i] = str.position[i];
			this->velocity[i] = str.velocity[i];
			this->bestPosition[i] = str.bestPosition[i];
		}
		for (int i = 0; i < numberOfObjectives; i++) {
			this->value[i] = str.value[i];
			this->bestValue[i] = str.bestValue[i];
		}
		//粒子可行性
		this->feasible = str.feasible;
		this->bestFeasible = str.bestFeasible;
		//权重
		this->c1 = str.c1;
		this->c2 = str.c2;
		this->weight = str.weight;
		this->vari = str.vari;
		this->functionPtr = str.functionPtr;
		this->maxVelocity = str.maxVelocity;
		//约束（在优化程序中进行设定） 
		this->constraits = str.constraits;
		//粒子被占优次数
		this->paretoDominance = str.paretoDominance;
		//粒子位置上界下界
		this->lowerBounds = str.lowerBounds;
		this->upperBounds = str.upperBounds;
		//粒子最大速度
		this->maxVelocity = str.maxVelocity;
	}
	return *this;
}

Particle::~Particle()
{
	//cout << "aa" << endl;
	delete[] position; 
	delete[] velocity;
	delete[] bestPosition;
	delete[] value;
	delete[] bestValue;
}

void Particle::inicializeParticle()
{
	//初始化粒子位置，速度=0，值最佳位置，合理性
	for (int i = 0; i < numberOfVariables; i++) {
		double random = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1); //生成0-1间的随机数 并转为double
		position[i] = lowerBounds[i] + (upperBounds[i] - lowerBounds[i]) * random;
		random = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1);
		velocity[i] = lowerBounds[i] + (upperBounds[i] - lowerBounds[i]) * random;
	}

	(*functionPtr)(this);

	bestFeasible = feasible;
	for (int i = 0; i < numberOfVariables; i++)
		bestPosition[i] = position[i];
	for (int i = 0; i < numberOfObjectives; i++)
		bestValue[i] = value[i];
}

void Particle::updateParticle(const double *globalBestPosition)
{
	//粒子位置更新
	double random1;
	double random2;
	double random3;
	double random4;

	for (int i = 0; i < numberOfVariables; i++) {
		random1 = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1);
		random2 = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1);
		velocity[i] = *weight * velocity[i] + c1 * random1 * (bestPosition[i] - position[i]) + c2 * random2 * (globalBestPosition[i] - position[i]);

		//判断速度是否过大
		if (fabs(velocity[i]) > maxVelocity[i])
			velocity[i] = maxVelocity[i] * velocity[i] / fabs(velocity[i]);

		//位置更新
		position[i] = position[i] + velocity[i];
		random3 = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1);
		random4 = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1);
		if (random3 < *vari)
		{
			position[i] = random4*(upperBounds[i] - lowerBounds[i]) + lowerBounds[i];
			//cout << 'a' ;
		}


		//判断位置是否过大
		if (position[i] >= upperBounds[i])
			position[i] = upperBounds[i];

		if (position[i] <= lowerBounds[i])
			position[i] = lowerBounds[i];
	}

	(*functionPtr)(this);

	for (int i = 0; i < numberOfObjectives; i++)
		value[i] += constraits;

	updateParticleBest();
}

const double *Particle::getBestPosition()
{
	return bestPosition;
}

const double *Particle::getBestValue()
{
	return bestValue;
}

bool Particle::getBestFeasible()
{
	return bestFeasible;
}

const double *Particle::getValue()
{
	return value;
}

void Particle::setValue(const double *_value)
{
//    for (int i = 0; i < numberOfObjectives; i++){
//        if(optimizeMode[i] == QObject::tr("Maximize")){
//            value[i] = _value[i];
//        }
//        else if(optimizeMode[i] == QObject::tr("Minimize")){
//            value[i] = 1/_value[i];
//        }
//        else{
//            qDebug() << "Error: Particle::setValue!";
//        }
//    }
    for(int i = 0; i < numberOfObjectives; ++i){
        value[i] = _value[i];
    }
}

void Particle::setConstraits(double _constraits)
{
	constraits = _constraits;
}

void Particle::setParetoDominance(int _paretoDominance)
{
	paretoDominance = _paretoDominance;
}

int Particle::getParetoDominance()
{
	return paretoDominance;
}




bool Particle::getFeasible()
{
	return feasible;
}

void Particle::setFeasible(bool _feasible)
{
	feasible = _feasible;
}

int Particle::getNumberOfVariables()
{
	return numberOfVariables;
}

int Particle::getNumberOfObjective()
{
	return numberOfObjectives;
}

const double *Particle::getPosition()
{
	return position;
}

void Particle::printParticle()
{
	//cout << "Position = ";
	for (int i = 0; i < numberOfVariables; i++)
		cout << position[i] << "   ";
	cout << endl;
	//cout << "Value = ";
	for (int i = 0; i < numberOfObjectives; i++)
		cout << value[i] << "   ";
	cout << endl;
	//cout << "Constraints = " << constraits << endl;
	//cout << "Feasible? " << (feasible ? "true" : "false") << endl;
}

void Particle::printParticleBest()
{
	cout << "Position = ";
	for (int i = 0; i < numberOfVariables; i++)
		cout << bestPosition[i] << "   ";
	cout << endl;
	cout << "Value = ";
	for (int i = 0; i < numberOfObjectives; i++)
		cout << bestValue[i] << "   ";
	cout << endl;
	cout << "Feasible? " << (bestFeasible ? "true" : "false") << endl;
}

///private
void Particle::updateParticleBest()
{
	if (feasible || !bestFeasible) {
		//判断支配性
		int dominance = 0;
		int nondominance = 0;
		int dom_less = 0;
		int dom_equal = 0;
		int dom_more = 0;
		for (int k = 0; k < numberOfObjectives; k++)
		{
			if (value[k] < bestValue[k])
				dom_less++;
			else if (value[k] == bestValue[k])
				dom_equal++;
			else
				dom_more++;
		}
		if ((dom_less == 0) && (dom_equal != numberOfObjectives))
			dominance++;
		else if ((dom_more == 0) && (dom_equal != numberOfObjectives))
			nondominance++;
		//判断最优值
		if (feasible != bestFeasible){
			for (int i = 0; i < numberOfVariables; i++)
				bestPosition[i] = position[i];
			for (int i = 0; i < numberOfObjectives; i++)
				bestValue[i] = value[i];
		}
		else if (nondominance == 1){
			for (int i = 0; i < numberOfVariables; i++)
				bestPosition[i] = position[i];
			for (int i = 0; i < numberOfObjectives; i++)
				bestValue[i] = value[i];
		}
		else if ((static_cast<double>(rand()) / (static_cast<double>(RAND_MAX)+1)) > 0.5){
			for (int i = 0; i < numberOfVariables; i++)
				bestPosition[i] = position[i];
			for (int i = 0; i < numberOfObjectives; i++)
				bestValue[i] = value[i];
		}
	}
}
