#ifndef SINGLEOBJECTCORE_H
#define SINGLEOBJECTCORE_H
#include <QString>
#include "singleobjectcore/particle.h"

class PSO
{
public:
    PSO();
    PSO(int _numberOfParticles,
        int _numberOfVariables,
        double *_lowerBounds,
        double *_upperBounds,
        double *_vmax,
        void (*_objectiveFunction)(Particle *),
        double _lowerWeight,
        double _upperWeight,
        int _maxIteration,
        double _c1,
        double _c2,
        double _threshold,
        QString _stoppingCriteria,
        QString _psoType);
    ~PSO();
    void optimize();
    void printBest();

private:
    void updateParticles();
    void updateGlobalBest();

    int numberOfParticles;
    int bestParticleIndex;
    double weight;
    double lowerWeight;
    double upperWeight;
    double c1;
    double c2;
    double threshold;
    int maxIteration;
    Particle **particles;
    QString psoType;
    QString stoppingCriteria;
};

#endif // SINGLEOBJECTCORE_H
