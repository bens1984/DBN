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

#define PRINT_DEBUG       // <- define to print matrix updates during Kalman Filter. Set ParticleFilter size to 1 or be very afraid!

#include "Particle.h"
#include <math.h>

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
    state.L = (ShapeQualities)(rand() % 3);    // L = {0...6}
    // sample R given L
    SampleR();
    
//    float alpha = 0.5;
    for (int i = 0; i < 5; i++)         // set continuous variables to 0 for a uniform start.
    {
        state.hiddenState[i][0] = 0;  // X
        state.hiddenState[i][1] = 0;  // V
        state.hiddenState[i][2] = 0;    // V0
//        state.hiddenState[i][3] = state.hiddenState[i][2];                // v0
        //        for (int j = 0; j < 3; j++)
        state.hiddenStateVariance[i].makeUnity();
        state.hiddenStateVariance[i] *= 5.0;
        state.y[i].fill(0); // initial observations
//        if (state.R[i] != 0 && state.M == 0)   // v0
//            state.v0[i] = exp(GetGaussianSample(log(state.v0[i]), v0Variance));
//        else
//            state.v0[i] = exp(GetGaussianSample(log(state.v0[i]), 1000));   // uniform initialiazition
//        double delta = (state.R[i] == 0 ? 0 : (state.R[i] == -1 ? -state.v0[i] : state.v0[i]));
//        state.hiddenState[i][2] = GetGaussianSample(alpha * state.hiddenState[i][2] + (1.0 - alpha) * delta, (1-alpha)/(1+alpha)); // V
    }
    weight = weight_normalized = 1;
}
void Particle::Resample(Particle* seed)   // creates a new particle that is distributed around this one.
{                                       //TODO: replace with a proven algorithm
    memcpy(&state, seed->GetState(), sizeof(state));    // load in seed state
    
    state.M = (ranf() < GESTURE_FREQUENCY);
    if (state.M == 0)
    {
//        state.L = state.L;
//        for (int i = 0; i < 5; i++)
//            state.R[i] = state.R[i];
    }
    else
    {
        state.L = (ShapeQualities)(rand() % 2);
        SampleR();
        for (int i = 0; i < 5; i++)
        {
            state.hiddenStateVariance[i].makeUnity();
            state.hiddenStateVariance[i] *= 5.0;
        }
    }
//    float alpha = 0.5;  // filtering constant for V update
//    for (int i = 0; i < 5; i++)
//    {
//        if (state.R[i] != 0 && state.M == 0)   // v0
//            state.hiddenState[i][2] = exp(GetGaussianSample(log(state.hiddenState[i][2]), state.hiddenStateVariance[i][2]));
//        else
//            state.hiddenState[i][2] = exp(GetGaussianSample(log(state.hiddenState[i][2]), 1000));   // uniform initialiazition
//        double delta = (state.R[i] == 0 ? 0 : (state.R[i] == -1 ? -state.hiddenState[i][2] : state.hiddenState[i][2]));
//        state.hiddenState[i][1] = GetGaussianSample(alpha * state.hiddenState[i][1] + (1.0 - alpha) * delta, (1-alpha)/(1+alpha)); // V
////        state.hiddenState[i][1] = state.hiddenState[i][0];  // previous X
//        state.hiddenState[i][0] += state.hiddenState[i][1];  // X
//    }
}
void Particle::SampleR()
{
    double r[5];
    for (int i = 0; i < 5; i++)
        r[i] = ranf();
    switch (state.L) {
        case rising:
            state.R[0] = (r[0] < 0.15 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = ((r[1] < 0.15 ? -1 : (r[1] < 0.9 ? 1 : 0)));
            state.R[2] = ((r[2] < 0.15 ? -1 : (r[2] < 0.9 ? 1 : 0)));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] > 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] > 0.9 ? 1 : 0)));
            break;
        case sinking:
            state.R[0] = (r[0] < 0.75 ? -1 : (r[0] < 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.75 ? -1 : (r[1] < 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.75 ? -1 : (r[2] < 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] >= 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.15 ? -1 : (r[4] >= 0.9 ? 1 : 0)));
            break;
        case advancing:
            state.R[0] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.1 ? -1 : (r[1] >= 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.1 ? -1 : (r[2] >= 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.01 ? -1 : (r[3] >= 0.02 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.1 ? -1 : (r[4] >= 0.9 ? 1 : 0)));
            break;
        case retreating:
            state.R[0] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.1 ? -1 : (r[1] >= 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.1 ? -1 : (r[2] >= 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.98 ? -1 : (r[3] >= 0.99 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.1 ? -1 : (r[4] >= 0.9 ? 1 : 0)));
            break;
        case spreading:
            state.R[0] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.1 ? -1 : (r[1] >= 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.1 ? -1 : (r[2] >= 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] >= 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.01 ? -1 : (r[4] >= 0.02 ? 1 : 0)));
            break;
        case enclosing:
            state.R[0] = (r[0] < 0.1 ? -1 : (r[0] >= 0.9 ? 1 : 0));
            state.R[1] = (r[1] < 0.1 ? -1 : (r[1] >= 0.9 ? 1 : 0));
            state.R[2] = (r[2] < 0.1 ? -1 : (r[2] >= 0.9 ? 1 : 0));
            state.R[3] = ((r[3] < 0.1 ? -1 : (r[3] >= 0.9 ? 1 : 0)));
            state.R[4] = ((r[4] < 0.98 ? -1 : (r[4] >= 0.99 ? 1 : 0)));
            break;
        case neutral:
            state.R[0] = (r[0] < 0.01 ? -1 : (r[0] >= 0.99 ? 1 : 0));
            state.R[1] = (r[1] < 0.01 ? -1 : (r[0] >= 0.99 ? 1 : 0));
            state.R[2] = (r[2] < 0.01 ? -1 : (r[0] >= 0.99 ? 1 : 0));
            state.R[3] = (r[3] < 0.01 ? -1 : (r[0] >= 0.99 ? 1 : 0));
            state.R[4] = (r[4] < 0.01 ? -1 : (r[0] >= 0.99 ? 1 : 0));
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
void Particle::SetWeights(double newWeight)  // reset to 1/N
{
    weight = weight_normalized = newWeight;
}
Particle* Particle::Copy()
{
    Particle* p = new Particle();
    dbnState* ds = p->GetState();
//    memcpy(ds, &state, sizeof(dbnState));
    
//    cout << "copy " << ds << " y: " << ds->y[0][0] << "::" << state.y[0][0] << endl;
    for (int i = 0; i < 5; i++)
//        for (int j = 0; j < state.y[i].getRows(); j++)
        {
            ds->y[i] = state.y[i];
            ds->hiddenState[i] = state.hiddenState[i];
            ds->hiddenStateVariance[i] = state.hiddenStateVariance[i];
        }
    p->SetWeights(weight_normalized);
    
    return p;
}
void Particle::Predict()
{
    CWSquareMatrix Q(3);
    Q.makeUnity(); 
//    Q = Q * 0.01;
    
    // transition for discrete states:
    state.M = 0; //(ranf() < GESTURE_FREQUENCY);
    if (state.M == 0)
    {
        //        state.L = state.L;    // L=L, R=R if M is 0
        //        state.R = state.R;
    }
    else
    {
        state.L = (ShapeQualities)(rand() % 7);
        SampleR();  // get R again!
//        state.hiddenState[i][2] = 0;      // reset V0 when M=1 ?
    }
    
    for (int i = 0; i < 5; i++)
    {        
        state.updateFunction[1][2] = state.R[i] * V_V0_INFLUENCE;      // set influence of R on V
        state.hiddenState[i] = state.updateFunction * (CWMatrix)state.hiddenState[i]; // + F * control;
        state.hiddenStateVariance[i] = state.updateFunction * state.hiddenStateVariance[i] * transpose(state.updateFunction) + Q; //*transpose(Q);
        
        state.hiddenState[i][0] += GetGaussianSample(0, state.hiddenStateVariance[i][0][0]);
        state.hiddenState[i][1] += GetGaussianSample(0, state.hiddenStateVariance[i][1][1]);
        state.hiddenState[i][2] += GetGaussianSample(0, state.hiddenStateVariance[i][2][2]); 
    }
    
//    // predict new X, V, V0... all others are considered to remain constant at this point (no transition model)
//    for (int i = 0; i < 5; i++)
//    {
//        state.updateFunction[1][2] = state.R[i] * 0.5;
//        state.hiddenState[i] = state.updateFunction * (CWMatrix)state.hiddenState[i];
//        state.hiddenStateVariance[i] = state.updateFunction * (CWMatrix)state.hiddenStateVariance[i] * transpose(state.updateFunction) + Q;
//        
//        if (state.R[i] != 0 && state.M == 0)   // v0
//            state.hiddenState[i][2] = exp(GetGaussianSample(log(state.hiddenState[i][2]), 1.0)); //state.hiddenStateVariance[i][2]));
//        else
//            state.hiddenState[i][2] = exp(GetGaussianSample(log(state.hiddenState[i][2]), 1000));   // uniform initialiazition
////        double delta = (state.R[i] == 0 ? 0 : (state.R[i] == -1 ? -state.hiddenState[i][2] : state.hiddenState[i][2]));
////        float alpha = 0.9;
////        float s = (1-0.5)/(1+0.5);  // ß from Swaminathan (7) and (8)
////        state.hiddenState[i][1] = GetGaussianSample(alpha * state.hiddenState[i][1] + (1.0 - alpha) * delta, s * 1.0); //state.hiddenStateVariance[i][1]);
////        
////        state.hiddenState[i][0] = state.hiddenState[i][0] + state.hiddenState[i][1]; // X_t-1 + V_t-1
////        state.hiddenStateVariance[i] = state.updateFunction * (CWMatrix)state.hiddenStateVariance[i];     // <- should this be done here or in Kalman step, below?
//    }
}
float Particle::CalculateWeight(vector<float> *y)   // observed = Y, return weight  ---  NEED TO REDO THIS. VARIANCE gets smaller with better accuracy...
{
    double temp;
    
    Predict();
    
//    cout << "----------";
//    double xWeight; //vWeight
    // weight measure:
    // p(y | y_t-1, discrete states)p(state | state_t-1, y) / q(state; state_t-1, y)
    
    // compare y to updatedX as importance function
    //   this is euclidean distance:
    //    float alpha = (1-0.5)/(1+0.5);  // ß from Swaminathan (7) and (8)
    weight = 0;
    for (int i = 0; i < 5; i++)
    {
//        if (i == 0)
//            cout << state.y[i][0] << ":" << state.y[i][1] << ":" << state.y[i][2] << " === ";
//        state.y[i][1] = y->at(i) - state.y[i][0];  // calculate dif
        state.y[i][0] = y->at(i);           // store current observation
//        state.y[i][2] = abs(state.y[i][1]); 
//        if (i == 0)
//            cout << state.y[i][0] << ":" << state.y[i][1] << ":" << state.y[i][2] << endl;

//        for (int j = 0; j < 3; j++)
//        {
            // this is the weight based on X:
//            if (state.hiddenStateVariance[i][0][0] != 0)
//                temp = 1.0 / (1 + abs(state.y[i][j] - state.hiddenState[i][j]) / state.hiddenStateVariance[i][j][j]);
            temp = 1.0 / pow(1.0 + abs(state.y[i][0] - state.hiddenState[i][0]),2);
//                temp = 1.0 - pow(state.y[i][j] - state.hiddenState[i][j], 2) * 0.5; // / state.hiddenStateVariance[i][j];
//            else
//                temp = 1.0 - pow(state.y[i][j] - state.hiddenState[i][j], 2); // / 0.001;    //
            temp = temp < 0 ? 0 : temp;
            weight += temp;
//            weight += xWeight;      // + vWeight;
//        }
    }
//    weight *= 0.071428571; // variance = E(deviation^2)/N-1. N = 15
//    if (weight != 0)
//        weight = 1.0 / weight;
//    if (weight_normalized != 0)
    weight *= weight_normalized;    // w = w_t-1 * p(y | z)
    
    return weight;
}
float Particle::NormalizeWeight(float sumWeight)     // normalize weight, set weight_normalized and return it
{
    if (sumWeight > 0)
        weight_normalized = weight / sumWeight;
    else
        weight_normalized = 0;
    
//    cout << "weight:" << weight_normalized << endl;
    return weight_normalized;
}
void Particle::KalmanForwardRecursion(bool print)
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
    int matSize = state.hiddenState[0].getRows();
	CWSquareMatrix R(1), vTemp(matSize);  // Q(matSize), 
    CWMatrix xTemp(matSize,1), yTemp;
	CWMatrix K(3,1);
    CWSquareMatrix S;  // K - temp matrix
    CWMatrix C(1, matSize);
    CWMatrix cTransp(C.getCols(), 1);
//    CWMatrix cTransp(C.getCols(), C.getRows()), control(matSize,1);
    CWSquareMatrix F(matSize), G(matSize);  // parameters/transforms for control signal
    CWSquareMatrix Ident(3);
    
//    control.fill(0);    // no control at the moment.
    R.makeUnity();
    R *= 0.01;
//    Q.makeUnity();
//    Q *= 0.01;
//    C.makeUnity();
    C.fill(0);
    C[0][0] = 1;
    cTransp.storeTranspose(C);
    F.fill(0);
    G.fill(0);
    Ident.makeUnity();
    
    for (int i = 0; i < 5; i++)
    {        
        xTemp = (CWMatrix)state.hiddenState[i];
//        xTemp = state.updateFunction * xTemp; // + F * control;       // <- this is already done in our prediction function
//        if (i == 0)
//            PrintMatrix("x=Ax+Fu", xTemp);
        vTemp = (CWMatrix)state.hiddenStateVariance[i];
//        vTemp = state.updateFunction * vTemp * transpose(state.updateFunction) + Q; //*transpose(Q);        
//        if (i == 0)
//            PrintMatrix("v=AvA'+Q", vTemp);
        
        S = C * vTemp * cTransp + R;//*transpose(R);       // de Freitas has this as R*R', but that is assuming a k x 1 matrix.
//        if (i == 0)
//        {
//            PrintMatrix("S=CvC'+R", S);
//            PrintMatrix("S-inverse", inv(S));
//        }
        CWSquareMatrix iSS = inv(S);
        CWMatrix tt = cTransp * iSS;
        K = vTemp * tt; //cTransp * inv(S);
//        if (i == 0)
//            PrintMatrix("y", (CWMatrix)state.y[i]);
        yTemp = (CWMatrix)state.y[i] - (C * xTemp); // + G * control);
//        if (i == 0)
//            PrintMatrix("y-Cx+Gu", yTemp);
//        if (i == 0)
//            PrintMatrix("K=vC'S^-1", K);
        xTemp = xTemp + K * yTemp;
        xTemp[2][0] = abs(xTemp[2][0]);  // keep V0 positive
//        if (i == 0)
//            PrintMatrix("x=x+Ky", xTemp);
        vTemp = (Ident - K * C) * vTemp;
//        vTemp = vTemp - K * C * vTemp;
        if (i == 0 && print)
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
