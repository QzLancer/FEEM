#include "sparticle.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <exception>
#include <QObject>
#include <QDebug>

using namespace std;

#include <cmath>

int SParticle::numberOfVariables = 1;

const double *SParticle::lowerBounds = NULL;

const double *SParticle::upperBounds = NULL;

const double *SParticle::maxVelocity = NULL;

double SParticle::c1 = 2;

double SParticle::c2 = 2;

const double *SParticle::weight = NULL;

void (*SParticle::functionPtr) (SParticle *);

SParticle::SParticle(int _numberOfVariables,
                const double *_lowerBounds,
                const double *_upperBounds,
                const double *_vmax,
                double _c1, double _c2,
                const double *_weight,
                void (*_functionPtr)(SParticle *),
                QString _optimizemode)
{
    lowerBounds = _lowerBounds;
    upperBounds = _upperBounds;

    numberOfVariables = _numberOfVariables;

    if (numberOfVariables <= 0)
        throw "Number of variables can't be less or equal than zero.";

    c1 = _c1;
    c2 = _c2;
    weight = _weight;
    functionPtr = _functionPtr;
    maxVelocity = _vmax;

    position = new double [numberOfVariables];
    velocity = new double [numberOfVariables] ;
    bestPosition = new double [numberOfVariables];

    optimizemode = _optimizemode;

    inicializeParticle();
}


SParticle::~SParticle ()
{
    delete [] position;
    delete [] velocity;
    delete [] bestPosition;
}

void SParticle::inicializeParticle ()
{
    for (int i = 0; i < numberOfVariables; i++) {
        double random = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1);
        position[i] = lowerBounds[i] + (upperBounds[i] - lowerBounds[i]) * random;
        velocity[i] = 0;
    }

    (*functionPtr)(this);

    bestValue = value;
    bestFeasible = feasible;
    for (int i = 0; i < numberOfVariables; i++)
        bestPosition[i] = position[i];

}

void SParticle::updateParticle (const double *globalBestPosition)
{
    double random1;
    double random2;

    for (int i = 0; i < numberOfVariables; i++) {
        random1 = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1);
        random2 = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX) + 1);
        velocity[i] = *weight * velocity[i] + c1 * random1 * (bestPosition[i] - position[i]) + c2 * random2 * (globalBestPosition[i] - position[i]);

        //tests if velocity doesn't exceed maximum velocity
        if (fabs(velocity[i]) > maxVelocity[i])
            velocity[i] = maxVelocity[i] * velocity[i] / fabs(velocity[i]);

        //update position
        position[i] = position[i] + velocity[i];

        //tests if position doesn't exceed upper or lower bounds
        if (position[i] >= upperBounds[i])
            position[i] = upperBounds[i];

        if (position[i] <= lowerBounds[i])
            position[i] = lowerBounds[i];
    }

    (*functionPtr)(this);

    value += constraits;

    updateParticleBest ();
}

const double *SParticle::getBestPosition ()
{
    return bestPosition;
}

double SParticle::getBestValue ()
{
    return bestValue;
}

bool SParticle::getBestFeasible ()
{
    return bestFeasible;
}

double SParticle::getValue ()
{
    return value;
}

void SParticle::setValue (double _value)
{
    if(optimizemode == QObject::tr("Maximize")){
        value = _value;
    }
    else if(optimizemode == QObject::tr("Maximize")){
        value = 1/_value;
    }
    else{
        qDebug() << "Error: SParticle::setValue! ";
    }
}

void SParticle::setConstraits (double _constraits)
{
    constraits = _constraits;
}

bool SParticle::getFeasible ()
{
    return feasible;
}

void SParticle::setFeasible (bool _feasible)
{
    feasible = _feasible;
}

void SParticle::printParticle ()
{
    cout << "Position = ";
    for (int i = 0; i < numberOfVariables; i++)
        cout << position[i] << "   ";
    cout << endl;
    cout << "Value = " << value << endl;
    cout << "Constraints = " << constraits << endl;
    cout << "Feasible? " << (feasible ? "true" : "false") << endl;
}

void SParticle::printParticleBest ()
{
    cout << "Position = ";
    for (int i = 0; i < numberOfVariables; i++)
        cout << bestPosition[i] << "   ";
    cout << endl;
    cout << "Value = " << bestValue << endl;
    cout << "Feasible? " << (bestFeasible ? "true" : "false") << endl;
}

int SParticle::getNumberOfVariables ()
{
    return SParticle::numberOfVariables;
}

const double *SParticle::getPosition ()
{
    return position;
}

///private
void SParticle::updateParticleBest ()
{
    //improve this conditional
    if (feasible || !bestFeasible) {
        if (feasible != bestFeasible || value <= bestValue) {
            bestValue = value;
            bestFeasible = feasible;

            for (int i = 0; i < numberOfVariables; i++)
                bestPosition[i] = position[i];
        }
    }
}
