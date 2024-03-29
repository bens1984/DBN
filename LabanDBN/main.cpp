//
//  main.cpp
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define FILTERSIZE 512

#include <iostream>
#include "OSCReceive.h"
#include "OSCSend.h"
#include "ParticleFilter.h"

void makeGraph(vector<Particle*>* filt)
{
    float graph[7], sum = 0;
    for (int i = 0; i < 7; i++)
        graph[i] = 0;
    
    for (std::vector<Particle*>::iterator it = filt->begin(); it != filt->end(); it++)
    {
        Particle* p = *it;
        dbnState* dbn = p->GetState();
        graph[(int)(dbn->L)] += pow(p->GetNormalizedWeight(), 2);
    }
    for (int i = 0; i < 7; i++)
        sum += graph[i];
    for (int i = 0; i < 7; i++)
        graph[i] /= sum;
    myOSCHandle::getSingleton()->oscSend("/graph", 7, &graph[0]);
//    float weights[100];
//    for (int i = 0; i < 100; i++)
//        weights[i] = filt->at(i)->GetNormalizedWeight();
//    myOSCHandle::getSingleton()->oscSend("/w", 100, &weights[0]);
}

int main (int argc, const char * argv[])
{
    ParticleFilter myFilter(FILTERSIZE);
    OSCReceive myOSC;
    
    myOSC.StartReception();
    
    // insert code here...
    std::cout << "Hello, World!\n";
    while (1)
    {
        OSCData data = myOSC.ReadAPacket();
        if (data.header != oscNothing)
        {
//            myFilter.Predict();
            myFilter.CalculateWeights(&(data.data));
            Particle* p = myFilter.GetDominantParticle();
            if (p != NULL)
            {
                float L = p->GetState()->L;
                myOSCHandle::getSingleton()->oscSend("/sq", 1, &L);
                dbnState* dbn = p->GetState();
                cout << endl << myFilter.GetEffectiveNumber() << " weight: " << p->GetNormalizedWeight() << " L:" << dbn->L << " R:[";
                for (int i = 0; i < 5; i++)
                    cout << dbn->R[i] << ",";
                cout << "] hiddenState:[";
                for (int i = 0; i < 3; i++)
                {
 //                   for (int j = 0; j < dbn->hiddenState[0].getRows(); j++)
                        cout << dbn->hiddenState[0][i] << ",";
                }
                cout << "] obs:[";
                
                for (int j = 0; j < 5; j++)
                    cout << dbn->y[j][0] << ",";
                cout << "] variance: [";
                for (int j = 0; j < 5; j++)
                    cout << dbn->hiddenStateVariance[j][0][0] << ",";
                cout << "]";
                
            } else
                cout << "no maximal particle found.";
            cout << endl;
            makeGraph(myFilter.GetParticles());
            if (myFilter.GetEffectiveNumber() < FILTERSIZE * N_thresh)
            {
                cout << "resampling" << endl;
                myFilter.Resample();
            }
            myFilter.ExactUpdate(&(data.data));
        }
    }
    
    myOSC.StopReception();
    return 0;
}

