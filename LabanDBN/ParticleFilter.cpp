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
void ParticleFilter::MultinomialResample()
{
    float cumulativeWeight[myParticles.size()];
    float weight = 0;
//    double newWeight = 1.0 / myParticles.size();
    for (int i = 0; i < myParticles.size(); i++)
        cumulativeWeight[i] = weight = weight + myParticles.at(i)->GetNormalizedWeight();
    
    std::vector<Particle*> newParticles;
    for (int i = 0; i < myParticles.size(); i++)
    {
        double u = i / (double)myParticles.size(); //ranf();  // get a uniform sample
        for (int j = 0; j < myParticles.size(); j++)
            if (u < cumulativeWeight[j])
            {
                newParticles.push_back(myParticles.at(j)->Copy());
                //                newParticles.at(newParticles.size()-1)->SetWeights(newWeight);
                break;  // j
            }
    }
    myParticles.erase(myParticles.begin(), myParticles.end());
    myParticles.insert(myParticles.begin(), newParticles.begin(), newParticles.end());
}
void ParticleFilter::SystematicResample()
{
    
}
void ParticleFilter::ResidualResample()
{
    std::vector<Particle*> newParticles;
    int count = myParticles.size();
    double q_residual[count]; 
    int N_babies[count];
    double babySum = 0;
    int i = 0;
    for (i = 0; i < count; i++)
//    for (std::vector<Particle*>::iterator it = myParticles.begin(); it != myParticles.end(); it++)
    {
        q_residual[i] = count * myParticles.at(i)->GetNormalizedWeight();
        babySum += N_babies[i] = fix(q_residual[i]);
    }
    // Residual number of particles to sample:
    int N_residual = count - babySum;
    cout << "N_residual " << N_residual << " ";
    if (N_residual != 0)
    {
        for (i = 0; i < count; i++)
            q_residual[i] = (q_residual[i] - N_babies[i]) / N_residual;
        
        float cumulativeDist[count];
        float weight = 0;
        for (i = 0; i < count; i++)
            cumulativeDist[i] = weight = weight + q_residual[i];
        // u = fliplr( cumprod( rand(1,N_residual) .^ (1. / (N_residual:-1:1) ) ) );
        float cProduct, cumulativeProduct[count];
        cProduct = 1;
        for (i = 0; i < N_residual; i++)
        {
            double exp = 1.0 / (double)(N_residual - i);
            double value = pow(ranf(), exp);
            cumulativeProduct[count - i - 1] = cProduct = cProduct * value;
        }
        int j = 0;
        for (i = 0; i < N_residual ; i++)
        {
            while (cumulativeProduct[i] > cumulativeDist[j])
                j++;
//            newParticles.push_back(myParticles.at(j)->Copy());
            N_babies[j]++;
        }
    }
    
    // now multiply/delete particles based on N_babies
//    int index = 0;
    for (i = 0; i < count; i++)
    {
//        if (N_babies[i] > 0)
            for (int j = 0; j < N_babies[i]; j++) //index; j < index+N_babies[i]-1; j++)
                newParticles.push_back(myParticles.at(i)->Copy());
//        index += N_babies[i];
    }
    
    myParticles.erase(myParticles.begin(), myParticles.end());
    myParticles.insert(myParticles.begin(), newParticles.begin(), newParticles.end());
}
void ParticleFilter::ImportanceResample()
{
    std::vector<Particle*> newParticles;
    // cumulative density function:
    double cumulativeWeight[myParticles.size()];
    double weight = 0;
    double newWeight = 1.0 / (double)myParticles.size();
    for (int i = 0; i < myParticles.size(); i++)
        cumulativeWeight[i] = weight = weight + myParticles.at(i)->GetNormalizedWeight();
    double start = ranf() * newWeight;
    int i = 0;
    for (int j = 0; j < myParticles.size(); j++)
    {
        double sample = start + newWeight * j;
        while (sample > cumulativeWeight[i]) {
            i++;
        }
        newParticles.push_back(myParticles.at(i)->Copy());
        newParticles.at(newParticles.size()-1)->SetWeights(newWeight);
    }
    myParticles.erase(myParticles.begin(), myParticles.end());
    myParticles.insert(myParticles.begin(), newParticles.begin(), newParticles.end());
}
void ParticleFilter::Resample()
{
    if (myParticles.size() == 0)
        return;
    
//    MultinomialResample();
//    ResidualResample();
    ImportanceResample();
    cout << "sampled " << myParticles.size() << " new samples" << endl;
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