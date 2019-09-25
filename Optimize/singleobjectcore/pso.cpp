#include "pso.h"
#include <QDebug>

PSO::PSO()
{

}

PSO::PSO(int _numberOfParticles, int _numberOfVariables, double *_lowerBounds, double *_upperBounds, double *_vmax, void (*_objectiveFunction)(Particle *), double _lowerWeight, double _upperWeight, int _maxIteration, double _c1, double _c2, double _threshold, QString _stoppingCriteria, QString _psoType)
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

    particles = new Particle *[numberOfParticles];

    for ( i = 0; i < numberOfParticles; i++)
        particles[i] = new Particle	(_numberOfVariables,
                                     _lowerBounds,
                                     _upperBounds,
                                     _vmax,
                                     _c1, _c2,
                                     &weight,
                                     _objectiveFunction);

    bestParticleIndex = 0;

    updateGlobalBest();
}

PSO::~PSO ()
{
    for (int i = 0; i < numberOfParticles; i++)
        delete particles[i];
    delete [] particles;

}

void PSO::updateParticles ()
{
    for (int i = 0; i < numberOfParticles; i++) {
        particles[i]->updateParticle(particles[bestParticleIndex]->getBestPosition());
    }
}

void PSO::updateGlobalBest ()
{
    for (int i = 0; i < numberOfParticles; i++)
        if (particles[i]->getFeasible() || !particles[bestParticleIndex]->getBestFeasible())
            if (particles[i]->getFeasible() != particles[bestParticleIndex]->getBestFeasible() || particles[i]->getValue() <= particles[bestParticleIndex]->getBestValue())
                bestParticleIndex = i;
}

void PSO::optimize ()
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

void PSO::printBest ()
{
    particles[bestParticleIndex]->printParticleBest();
}
