//
//  Particle.h
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#define ranf() \
((double)random()/(1.0+(double)RAND_MAX)) // Uniform from interval [0,1) */

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
    CWVector hiddenState[5];
    CWVector hiddenStateVariance[5];
    double v0[5];
    CWVector y[5];
    double predX[5], predV[5], predV0[5];
    
    CWSquareMatrix updateFunction;
    
    dbnState()
    {
        for (int i = 0; i < 5; i++)
        {
            hiddenState[i].dimension(4);
            hiddenStateVariance[i].dimension(4);
            y[i].dimension(4);
        }
        updateFunction.dimension(4);
        updateFunction[0][0] = 1; updateFunction[0][1] = 0; updateFunction[0][2] = 1; updateFunction[0][3] = 0;   // to update x based on prev x and v
        updateFunction[1][0] = 1; updateFunction[1][1] = 0; updateFunction[1][2] = 0; updateFunction[1][3] = 0;   // update prev x
        updateFunction[2][0] = 0; updateFunction[2][1] = 0; updateFunction[2][2] = 0.5; updateFunction[2][3] = 0.5;  // update v
        updateFunction[3][0] = 0; updateFunction[3][1] = 0; updateFunction[3][2] = 0; updateFunction[3][3] = 1;     // retain v0
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
    void Resample(dbnState* seed);   // resample this particle around the seed particle
    
    dbnState* GetState();
    float GetWeight();
    float GetNormalizedWeight();
    
    void Predict();                         // predict next time step
    float CalculateWeight(vector<float> *y);                    // calculate the importance of this particle
    float NormalizeWeight(float sumWeight);     // normalize weight, set weight_normalized and return it
    void KalmanForwardRecursion(vector<float> *y);      // exact step to update continuous state variables
};