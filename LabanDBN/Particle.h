//
//  Particle.h
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#pragma once

// defines to adjust filter functionality
// sampling - resample algorithm, either IMPORTANCE or RESIDUAL
//#define sampling IMPORTANCE 
#define sampling RESIDUAL
//          derive observed V and V0 from Y and use in Kalman update?
#define DERIVE_Y_V
//           consider the probability of p(R | L) in the weighting?
#define R_PROB
//          amount of noise modeled in transition functions
#define MODEL_NOISE 0.01
#define OBSERVATION_NOISE 0.01
// Gesture Frequency - how often M is reset to 1
      // 1 over number of frames per typical gesture (1/30 = 0.03)
#define GESTURE_FREQUENCY 0.1
// how strongly R (the shape quality) influences the Kalman prediction for V, and thus X
#define V_V0_INFLUENCE 0.985          
// ----------------

#define ranf() \
((double)random()/(1.0+(double)RAND_MAX)) // Uniform from interval [0,1) */

#define twoPi 6.283185307179586476925286766559005768394 // 2 * pi
#define e 2.718281828459045235360287471352662497757 // e

#include <vector>
#include <math.h>
#include "cwmtx.h" // <- part of the CWTMatrix library

#define fix(d) (d > 0 ? floor(d) : ceil(d))

using namespace std;

using namespace CwMtx;
typedef CWTVector<double> CWVector;

extern const float R_PROBABILITY[];

enum ShapeQualities {
    rising, sinking, advancing, retreating, spreading, enclosing, neutral
};

struct dbnState {
    int M;
    ShapeQualities L;
    int R[5];
    CWVector hiddenState[5], y[5];
    CWSquareMatrix hiddenStateVariance[5];
    float* probabilityR;
    
    CWSquareMatrix updateFunction;
    
    dbnState()
    {
        
        for (int i = 0; i < 5; i++)
        {
            hiddenState[i].dimension(3);
            hiddenState[i].fill(0);
            hiddenStateVariance[i].dimension(3);
            y[i].dimension(3);
            y[i].fill(0);
        }
        updateFunction.dimension(3);
//        updateFunction.makeUnity();
        updateFunction[0][0] = 1; updateFunction[0][1] = 1; updateFunction[0][2] = 0;   // to update x based on prev x and v
        updateFunction[1][0] = 0; updateFunction[1][1] = 1.0 - V_V0_INFLUENCE; updateFunction[1][2] = V_V0_INFLUENCE;  // update v  // 1; updateFunction[1][2] = 0; //
        updateFunction[2][0] = 0; updateFunction[2][1] = 0; updateFunction[2][2] = 1;     // retain v0
    }
};

class Particle
{
private:
    dbnState state;
    double weight;   // P(state | Y) or whatever
    double weight_normalized;    // normalized weight
    double baseProbability;
    
    double GetGaussianSample(double mean, double variance)
    {
        double u, v, x; //, y;
        u = ranf();
        v = ranf();
        
        u = sqrt(-2 * log (u) );
        v = twoPi * v;
        
        x = u * cos(v) * variance + mean;
//        y = u * sin(v) * variance + mean;
        
        return x;
        
//        double x1, x2, w, y1, y2;
//        
//        do {
//            x1 = 2.0 * ranf() - 1.0;
//            x2 = 2.0 * ranf() - 1.0;
//            w = x1 * x1 + x2 * x2;
//        } while ( w >= 1.0 );
//        
//        w = sqrt( (-2.0 * log( w ) ) / w );
//        y1 = x1 * w;
//        y2 = x2 * w;
//        return y1;  // could return y2 also
    }
    
    void PrintMatrix(string name, const CWMatrix &mat);
    
    void SampleR(); // sample the R values based on L
public:
    Particle();
    ~Particle();
    
    void Initialize();  // randomly generate a new particle
    void Resample();   // predict state values
    
    dbnState* GetState();
    double GetWeight();
    double GetNormalizedWeight();
    void SetWeights(double newWeight);  // reset to 1/N
    
    Particle* Copy();
    
    void Predict();                         // predict next time step
    double CalculateWeight(vector<float> *y);                    // calculate the importance of this particle
    double NormalizeWeight(double sumWeight);     // normalize weight, set weight_normalized and return it
    void KalmanForwardRecursion(bool print);      // exact step to update continuous state variables
};