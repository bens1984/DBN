//
//  ParticleFilter.h
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Particle.h"
#include <vector>

using namespace std;

class ParticleFilter
{
private:
    int     filterSize;
    vector<Particle*> myParticles;   // particles maintained for this filter
    // PDF ... how can this be represented? both for initial state, for evaluating weights, and selecting new particles
    
public:
    ParticleFilter(int size);
    ~ParticleFilter();
    
    void InitParticles(int count);  // init and randomly sample Count particles 
    void Predict();
    void CalculateWeights(vector<float>* y);
    void Resample();
    void ExactUpdate(vector<float>* y); // update continuous states with Kalman Filter
    Particle* GetDominantParticle();
    double GetEffectiveNumber();        // calculate the effective number of particles
    vector<Particle*>* GetParticles();
};