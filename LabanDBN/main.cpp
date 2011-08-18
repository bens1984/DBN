//
//  main.cpp
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "OSCReceive.h"
#include "ParticleFilter.h"

int main (int argc, const char * argv[])
{
    ParticleFilter myFilter(1);
    OSCReceive myOSC;
    
    myOSC.StartReception();
    
    // insert code here...
    std::cout << "Hello, World!\n";
    while (1)
    {
        OSCData data = myOSC.ReadAPacket();
        if (data.header != oscNothing)
        {
            myFilter.CalculateWeights(&(data.data));
//            Particle* p = myFilter.GetDominantParticle();
//            dbnState* dbn = p->GetState();
//            cout << p->GetNormalizedWeight() << " L:" << dbn->L << " R:[";
//            for (int i = 0; i < 5; i++)
//                cout << dbn->R[i] << ",";
//            cout << "] hiddenState:";
//            for (int i = 0; i < 5; i++)
//            {
//                cout << "[";
//                for (int j = 0; j < 4; j++)
//                    cout << dbn->hiddenState[i][j] << ",";
//                cout << "] ";
//            }
//            cout << endl;
            myFilter.Resample();
            myFilter.ExactUpdate(&(data.data));
        }
    }
    
    myOSC.StopReception();
    return 0;
}

