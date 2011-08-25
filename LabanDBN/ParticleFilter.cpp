//
//  ParticleFilter.cpp
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ParticleFilter.h"

ParticleFilter::ParticleFilter(int size)
{
    InitParticles(size);
}
ParticleFilter::~ParticleFilter()
{
    for (std::vector<Particle*>::iterator it = myParticles.end(); it != myParticles.begin(); it--)
        delete *it;
}

void ParticleFilter::InitParticles(int count)  // init and randomly sample Count particles 
{
    if (count == 0)
        return;
    
    float newWeight = 1.0 / count;
    for (int i = 0; i < count; i++)
    {
        myParticles.push_back(new Particle());
        myParticles.at(i)->SetWeights(newWeight);
    }
}
void ParticleFilter::Predict()
{
    for (int i = 0; i < myParticles.size(); i++)
        myParticles.at(i)->Predict();
}
void ParticleFilter::CalculateWeights(vector<float>* y)
{
    double total = 0;
    for (int i = 0; i < myParticles.size(); i++)
        total += myParticles.at(i)->CalculateWeight(y);
    
    for (int i = 0; i < myParticles.size(); i++)
        myParticles.at(i)->NormalizeWeight(total);
}
void ParticleFilter::Resample()
{
    if (myParticles.size() == 0)
        return;
    
    float cumlativeWeight[myParticles.size()];
    float weight = 0;
    double newWeight = 1.0 / myParticles.size();
    for (int i = 0; i < myParticles.size(); i++)
        cumlativeWeight[i] = weight = weight + myParticles.at(i)->GetNormalizedWeight();
    
    std::vector<Particle*> newParticles;
    for (int i = 0; i < myParticles.size(); i++)
    {
        double u = ranf();  // get a uniform sample
        for (int j = 0; j < myParticles.size(); j++)
            if (u < cumlativeWeight[j])
            {
                newParticles.push_back(myParticles.at(j)->Copy());
//                newParticles.at(newParticles.size()-1)->SetWeights(newWeight);
                break;  // j
            }
    }
    myParticles.erase(myParticles.begin(), myParticles.end());
    myParticles.insert(myParticles.begin(), newParticles.begin(), newParticles.end());
}
Particle* ParticleFilter::GetDominantParticle()
{
    double max = -1;
    int maxIndex = -1;
    for (int i = 0; i < myParticles.size(); i++)
    {
        if (myParticles.at(i)->GetNormalizedWeight() > max)
        {
            maxIndex = i;
            max = myParticles.at(i)->GetNormalizedWeight();
        }
    }
    if (maxIndex > -1)
        return myParticles.at(maxIndex);
    else
        return NULL;
}
void ParticleFilter::ExactUpdate(vector<float>* y)
{
    for (int i = 0; i < myParticles.size(); i++)
        myParticles.at(i)->KalmanForwardRecursion(i==0);
}
double ParticleFilter::GetEffectiveNumber()        // calculate the effective number of particles
{
    double effective = 0;
    for (int i = 0; i < myParticles.size(); i++)
        effective += pow(myParticles.at(i)->GetNormalizedWeight(), 2);
    if (effective > 0)
        effective = 1 / effective;
    return effective;
}
std::vector<Particle*>* ParticleFilter::GetParticles()
{
    return &myParticles;
}