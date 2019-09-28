#include "spso.h"
#include <QDebug>

SPSO::SPSO()
{

}

SPSO::SPSO(int _numberOfParticles, int _numberOfVariables, double *_lowerBounds, double *_upperBounds, double *_vmax, void (*_objectiveFunction)(SParticle *), double _lowerWeight, double _upperWeight, int _maxIteration, double _c1, double _c2, double _threshold, QString _optimizemode, QString _stoppingCriteria, QString _psoType)
{
    int i = 0;

    numberOfParticles = _numberOfParticles;
    maxIteration = _maxIteration;
    upperWeight = _upperWeight;
    lowerWeight = _lowerWeight;
    stoppingCriteria = _stoppingCriteria;
    psoType = _psoType;
    threshold = _threshold;

    if (numberOfParticles <= 0)
        throw "Number of particles can't be less or equal than zero.";

    particles = new SParticle *[numberOfParticles];

    for ( i = 0; i < numberOfParticles; i++)
        particles[i] = new SParticle	(_numberOfVariables,
                                     _lowerBounds,
                                     _upperBounds,
                                     _vmax,
                                     _c1, _c2,
                                     &weight,
                                     _objectiveFunction,
                                     _optimizemode);

    bestParticleIndex = 0;

    updateGlobalBest();
}

SPSO::~SPSO ()
{
    for (int i = 0; i < numberOfParticles; i++)
        delete particles[i];
    delete [] particles;

}

void SPSO::updateParticles ()
{
    for (int i = 0; i < numberOfParticles; i++) {
        particles[i]->updateParticle(particles[bestParticleIndex]->getBestPosition());
    }
}

void SPSO::updateGlobalBest ()
{
    for (int i = 0; i < numberOfParticles; i++)
        if (particles[i]->getFeasible() || !particles[bestParticleIndex]->getBestFeasible())
            if (particles[i]->getFeasible() != particles[bestParticleIndex]->getBestFeasible() || particles[i]->getValue() <= particles[bestParticleIndex]->getBestValue())
                bestParticleIndex = i;
}

void SPSO::optimize ()
{
    int i = 0;

        while ( i < maxIteration)
            {
                updateParticles ();
                updateGlobalBest ();
                i++;
                weight = upperWeight - i / maxIteration * (upperWeight - lowerWeight);
            }



    qDebug() << i;
}

void SPSO::printBest ()
{
    particles[bestParticleIndex]->printParticleBest();
}
