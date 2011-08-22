//
//  Particle.cpp
//  LabanDBN
//
//  Created by Ben Smith on 7/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
/*
        Init particle (based on initial distribution or previous particle)
        Sample M, L, R discrete states
        Sample X from Y, V from X, Xt-1, V0 from V given R
 
        Predict X_t+1 using transition matrix/function.
            - "Compute predictive densities for minimum stats."
        Evaluate importance weights - calculate likelihood of predicted values based on observations (mismatch between obs and pred?)
 
        Resample particles.
 
        Kalman Filter - "use one step of the Kalman recursion to compute the minimum stats"
 
 */

//#define PRINT_DEBUG       // <- define to print matrix updates during Kalman Filter. Set ParticleFilter size to 1 or be very afraid!

#include "Particle.h"
#include <math.h>

#define v0Variance 10.0

Particle::Particle() : weight(0), weight_normalized(0)
{    
    Initialize();
}
Particle::~Particle()
{
    
}

void Particle::Initialize()  // sample a new particle given no preexisting state                // should this be based on Y current observations?
{
    // how is state defined? what is the initial distribution?
    state.M = 0; // M = 0
    state.L = (ShapeQualities)(rand() % 2);    // L = {0...6}
    // sample R given L
    SampleR();
    
//    float alpha = 0.5;
    for (int i = 0; i < 5; i++)         // set continuous variables to 0 for a uniform start.
    {
        state.hiddenState[i][0] = 0;  // X
        state.hiddenState[i][1] = 0;  // previous X
        state.hiddenState[i][2] = 0;    // V
        state.hiddenState[i][3] = state.hiddenState[i][2];                // v0
        for (int j = 0; j < 4; j++)
            state.hiddenStateVariance[i][j] = 0;
        state.y[i].fill(0); // initial observations
//        if (state.R[i] != 0 && state.M == 0)   // v0
//            state.v0[i] = exp(GetGaussianSample(log(state.v0[i]), v0Variance));
//        else
//            state.v0[i] = exp(GetGaussianSample(log(state.v0[i]), 1000));   // uniform initialiazition
//        double delta = (state.R[i] == 0 ? 0 : (state.R[i] == -1 ? -state.v0[i] : state.v0[i]));
//        state.hiddenState[i][2] = GetGaussianSample(alpha * state.hiddenState[i][2] + (1.0 - alpha) * delta, (1-alpha)/(1+alpha)); // V
    }
    // calculate the weight right away
    
}
void Particle::Resample(dbnState* seed)   // creates a new particle that is distributed around this one.
{                                       //TODO: replace with a proven algorithm
    state.M = (ranf() > 0.99);
    if (state.M == 0)
    {
        state.L = seed->L;
        for (int i = 0; i < 5; i++)
            state.R[i] = seed->R[i];
    }
    else
    {
        state.L = (ShapeQualities)(rand() % 2);
        SampleR();
    }
    float alpha = 0.5;  // filtering constant for V update
    for (int i = 0; i < 5; i++)
    {
        if (state.R[i] != 0 && state.M == 0)   // v0
            state.v0[i] = exp(GetGaussianSample(log(state.v0[i]), v0Variance));
        else
            state.v0[i] = exp(log(state.v0[i] + (ranf() * 100 - 50)));   // uniform initialiazition
        double delta = (state.R[i] == 0 ? 0 : (state.R[i] == -1 ? -state.v0[i] : state.v0[i]));
        state.hiddenState[i][2] = GetGaussianSample(alpha * state.hiddenState[i][2] + (1.0 - alpha) * delta, (1-alpha)/(1+alpha)); // V
        state.hiddenState[i][1] = state.hiddenState[i][0];  // previous X
        state.hiddenState[i][0] += state.hiddenState[i][2];  // X
    }
}
void Particle::SampleR()
{
    double r[5];
    for (int i = 0; i < 5; i++)
        r[i] = ranf();
    switch (state.L) {
        case rising:
            state.R[0] = (r[0] < 0.15 ? -1 : (r[0] < 0.85 ? 1 : 0));
            state.R[1] = ((r[1] < 0.15 ? -1 : (r[0] < 0.85 ? 1 : 0)));
            state.R[2] = ((r[2] < 0.15 ? -1 : (r[0] < 0.85 ? 1 : 0)));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case sinking:
            state.R[0] = (r[0] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case advancing:
            state.R[0] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[1] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[2] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case retreating:
            state.R[0] = (r[0] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case spreading:
            state.R[0] = (r[0] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case enclosing:
            state.R[0] = (r[0] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case neutral:
            state.R[0] = (r[0] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
            
        default:
            break;
    }
}
dbnState* Particle::GetState()
{
    return &state;
}
float Particle::GetWeight()
{
    return weight;
}
float Particle::GetNormalizedWeight()
{
    return weight_normalized;
}
void Particle::Predict()
{
    // predict new X, V, V0... all others are considered to remain constant at this point (no transition model)
    for (int i = 0; i < 5; i++)
    {
//        state.predV0[i] = state.v0[i];
        if (state.R[i] != 0 && state.M == 0)   // v0
            state.predV0[i] = exp(GetGaussianSample(log(state.v0[i]), v0Variance));
        else
            state.predV0[i] = exp(GetGaussianSample(log(state.v0[i]), 1000));   // uniform initialiazition
        double delta = (state.R[i] == 0 ? 0 : (state.R[i] == -1 ? -state.v0[i] : state.v0[i]));
        float alpha = 0.5;
        state.predV[i] = alpha * state.hiddenState[i][2] + (1.0 - alpha) * delta;
        
//        if (state.R[i] != 0 && state.M == 0)   // v0
//            state.predV0[i] = exp(GetGaussianSample(log(state.v0[i]), v0Variance));
//        else
//            state.predV0[i] = exp(log(state.v0[i] + (ranf() * 100 - 50)));   // uniform initialiazition
//        state.predV[i] = GetGaussianSample(alpha * state.hiddenState[i][2] + (1.0 - alpha) * delta, (1-alpha)/(1+alpha)); // V
        
        state.predX[i] = state.hiddenState[i][0] + state.predV[i]; // X_t-1 + V_t-1
    }
}
float Particle::CalculateWeight(vector<float> *y)   // observed = Y, return weight
{
    double vWeight, xWeight;
    // weight measure:
    // p(y | y_t-1, discrete states)p(state | state_t-1, y) / q(state; state_t-1, y)
    
    // compare y to updatedX as importance function
    //   this is euclidean distance:
    float alpha = (1-0.5)/(1+0.5);  // ß from Swaminathan (7) and (8)
    weight = 0;
    for (int i = 0; i < 5; i++)
    {
        double xVariance = state.hiddenStateVariance[i][0];
        double vVariance = alpha * state.hiddenStateVariance[i][2];
        if (i == 0)
            cout << "variance:" << xVariance << ", " << vVariance << endl;
        state.y[i][1] = state.y[i][0];      // store previous observation
        state.y[i][0] = y->at(i);           // store current observation
        state.y[i][2] = state.y[i][0] - state.y[i][1];  // calculate dif
        state.y[i][3] = abs(state.y[i][2]);             // abs of dif
        // based on previous observation is this new observation accurate?
        // based on previous state and observations did we predict accurately?
        
        // this is the weight based on V:
        double observedV = y->at(i) - state.hiddenState[i][0];
        if (vVariance != 0)
            vWeight = abs(state.predV[i] - observedV) / vVariance;
        else
            vWeight = 0;
        // this is the weight based on X:
        if (xVariance != 0)
            xWeight = abs(y->at(i) - state.predX[i]) / xVariance;
        else
            xWeight = 0;
        weight += vWeight + xWeight;
        if (i == 0)
            cout << "weight:" << weight << endl;
    }
//    if (weight != 0)
//        weight = 1.0 / weight;
    
    return weight;
}
float Particle::NormalizeWeight(float sumWeight)     // normalize weight, set weight_normalized and return it
{
    if (sumWeight != 0)
        weight_normalized = weight / sumWeight;
    return weight_normalized;
}
void Particle::KalmanForwardRecursion(vector<float> *observations)
{
    // k - size of X (hidden state)
    // p - size of Y (observations)
	// Y - a vector of observation, (1 by p)
	// updateFunction (A) - tranisition, model update matrix, (k by k)
	// C - transform matrix from Y to X (hidden state matrix), (p by k)
	// Q - gaussian noise during model step, (k by k)
	// R - gaussian noise during observation step, (p by p)
    // S - variance of observations (I think)
    // xTemp = working hidden state estimation
    // vTemp - working hidden state estimation variance
	CWSquareMatrix Q(4), R(4);
    CWMatrix xTemp(4,1), vTemp(4,1), yTemp(4,1);
	CWSquareMatrix C(4), K, S;  // K - temp matrix
    CWMatrix cTransp(C.getCols(), C.getRows()), control(4,1);
    CWSquareMatrix F(4), G(4);  // parameters/transforms for control signal
    
    control.fill(0);    // no control at the moment.
    R.makeUnity();
    Q.makeUnity();
    C.makeUnity();
    F.makeUnity();
    G.makeUnity();
    
    // transition for discrete states:
    state.M = state.M;
    if (state.M == 0)
    {
        state.L = state.L;    // L=L, R=R if M is 0
//        state.R = state.R;
    }
    else
    {
        state.L = (ShapeQualities)(rand() % 2);
        SampleR();  // get R again!
    }
    
    for (int i = 0; i < 5; i++)
    {
        state.updateFunction[2][3] = (state.R[i] == 1 ? 0.5 : (state.R[i] == -1 ? -0.5 : 0)); // update the updateFunction according to the state functions
        if (state.M == 0 && state.R != 0)
            state.updateFunction[3][3] = 1;
        else
        {
            state.updateFunction[3][3] = 0;   // Vi_00 from Swaminathan - is never defined, probably the initial state
        }
        
        xTemp = state.hiddenState[i];
        xTemp = state.updateFunction * xTemp + F * control;
        if (i == 0)
            PrintMatrix("x=Ax+Fu", xTemp);
        vTemp = state.hiddenStateVariance[i];
        vTemp = state.updateFunction * vTemp * transpose(state.updateFunction) + Q;     // in all the examples both X and V use the same A (update function). Do we have to?
        if (i == 0)
            PrintMatrix("v=AvA'+Q", vTemp);
        
//        PostMatrix("xTemp", &xTemp);
//        PostMatrix("vTemp", &vTemp[i]);
        cTransp.storeTranspose(C);
        S = C * vTemp * cTransp + R;
        if (i == 0)
            PrintMatrix("S=CvC'+R", S);
//        PostMatrix("cTemp", &cTemp);
        K = vTemp * cTransp * inv(S);
        //        PostMatrix("K", &K);
        if (i == 0)
            PrintMatrix("y", (CWMatrix)state.y[i]);
        yTemp = (CWMatrix)state.y[i] - (C * xTemp + G * control);
        if (i == 0)
            PrintMatrix("y-Cx+Gu", yTemp);
        xTemp = xTemp + K * yTemp;
        if (i == 0)
            PrintMatrix("x=x+Ky", xTemp);
//        PostMatrix("xSequence", &xSequence[i]);
        vTemp = vTemp - K * C * vTemp;
        if (i == 0)
            PrintMatrix("v=v-KCv", vTemp);
        // store changes!
        state.hiddenState[i] = xTemp;
        state.hiddenStateVariance[i] = vTemp;
    }
}

void Particle::PrintMatrix(string name, const CWMatrix &mat)
{
#ifdef PRINT_DEBUG
    cout << name << ":";
    for (int i = 0; i < mat.getCols(); i++) {
        cout << "[";
        for (int j = 0; j < mat.getRows(); j++)
            cout << mat[i][j] << ",";
        cout << "] ";
    }
    cout << endl;
#endif
}
