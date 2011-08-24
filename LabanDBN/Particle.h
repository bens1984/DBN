//
//  Particle.h
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#define ranf() \
((double)random()/(1.0+(double)RAND_MAX)) // Uniform from interval [0,1) */

#define GESTURE_FREQUENCY 0.03      // 1 over number of frames per typical gesture (1/30 = 0.03)

#define V_V0_INFLUENCE 0.95          // how strongly R (the shape quality) influences the Kalman prediction for V, and thus X

#include <vector>
#include <math.h>
#include "cwmtx.h" // <- part of the CWTMatrix library

using namespace std;

using namespace CwMtx;
typedef CWTVector<double> CWVector;

enum ShapeQualities {
    rising, sinking, advancing, retreating, spreading, enclosing, neutral
};

struct dbnState {
    int M;
    ShapeQualities L;
    int R[5];
    CWVector hiddenState[5], y[5];
    CWSquareMatrix hiddenStateVariance[5];
    
    CWSquareMatrix updateFunction;
    
    dbnState()
    {
        for (int i = 0; i < 5; i++)
        {
            hiddenState[i].dimension(3);
            hiddenState[i].fill(0);
            hiddenStateVariance[i].dimension(3);
            y[i].dimension(1);
            y[i].fill(0);
        }
        updateFunction.dimension(3);
//        updateFunction.makeUnity();
        updateFunction[0][0] = 1; updateFunction[0][1] = 1; updateFunction[0][2] = 0;   // to update x based on prev x and v
        updateFunction[1][0] = 0; updateFunction[1][1] = 1.0 - V_V0_INFLUENCE; updateFunction[1][2] = V_V0_INFLUENCE;  // update v
        updateFunction[2][0] = 0; updateFunction[2][1] = 0; updateFunction[2][2] = 1;     // retain v0
    }
};

class Particle
{
private:
    dbnState state;
    float weight;   // P(state | Y) or whatever
    float weight_normalized;    // normalized weight
    
    double GetGaussianSample(double mean, double variance)
    {
        double x1, x2, w, y1, y2;
        
        do {
            x1 = 2.0 * ranf() - 1.0;
            x2 = 2.0 * ranf() - 1.0;
            w = x1 * x1 + x2 * x2;
        } while ( w >= 1.0 );
        
        w = sqrt( (-2.0 * log( w ) ) / w );
        y1 = x1 * w;
        y2 = x2 * w;
        return y1;  // could return y2 also
    }
    
    void PrintMatrix(string name, const CWMatrix &mat);
    
    void SampleR(); // sample the R values based on L
public:
    Particle();
    ~Particle();
    
    void Initialize();  // randomly generate a new particle
    void Resample(Particle* seed);   // resample this particle around the seed particle
    
    dbnState* GetState();
    float GetWeight();
    float GetNormalizedWeight();
    void SetWeights(double newWeight);  // reset to 1/N
    
    Particle* Copy();
    
    void Predict();                         // predict next time step
    float CalculateWeight(vector<float> *y);                    // calculate the importance of this particle
    float NormalizeWeight(float sumWeight);     // normalize weight, set weight_normalized and return it
    void KalmanForwardRecursion(bool print);      // exact step to update continuous state variables
};