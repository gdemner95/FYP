//
//  SynthPlugin.h
//  TestSynthAU
//
//  Software Synthesizer template project for UWE AMT/CMT students.
//

#ifndef __SynthPlugin_h__
#define __SynthPlugin_h__

#include "PluginProcessor.h"
#include "SynthExtra.h"
#include <sstream>

//===================================================================================
/** An example STK-voice, based on a sine wave generator                           */
class MyVoice : public Voice
{
public:
    int iSilenceCount;
    
    void onStartNote (const int pitch, const float velocity);
    bool onStopNote ();
    
    //    void onPitchWheel (const int value);
    //    void onControlChange (const int controller, const int value);
    
    bool process (float** outputBuffer, int numChannels, int numSamples);
    
private:
    //array of signal generators
    Buffer signalGenerator[8];
    Envelope noteOffEnv;
    int pitch;
    float fLevel;
};

struct VelRange
{
    const int randomNumbers[10] = {
        2,
        4,
        3,
        5,
        3,
        1,
        3,
        4,
        2,
        1,
    };
    Buffer* getNextSample(){
        int chosen = randomNumbers[rand() % 10];
        printf("Sampl chosen");
        return &samples[chosen];
    };
    
    Buffer samples[6];
    
private:
    
};

struct Drum
{
    VelRange* getVelRange(int velocity){
        if (velocity >= 0 & velocity < 21){
            //play lowest velocity for passed timbre
            return &velocities[0];
        }
        else if (velocity >= 21 & velocity < 40){
            return &velocities[1];
        }
        else if (velocity >= 41 & velocity < 60){
            return &velocities[2];
        }
        else if (velocity >= 61 & velocity < 80){
            return &velocities[3];
        }
        else if (velocity >= 81 & velocity < 90){
            return &velocities[4];
        }
        else
        {
            //play highest velocity for passed timbre
            return &velocities[5];
        }
    }
    
    VelRange velocities[6];
};
struct CymbalMics
{
    Drum mics[5];
};
class MySynth : public Synth
{
public:
    MySynth() : Synth() {
        initialise();
    }
    
    void initialise ();
    void postProcess (float** outputBuffer, int numChannels, int numSamples);
    
    const Buffer* getBuffer(int timbre, float velocity){
        velocity *= 127;
        return buffer[timbre].getVelRange(velocity)->getNextSample();
    }
    const Buffer* getCymbalBuffer(int timbre, float velocity, int mics){
        velocity *= 127;
        
        return cymbals[timbre].mics[mics].getVelRange(velocity)->getNextSample();
        
    }
    float* pSubmix[19];
    
private:
    // Insert synthesizer variables here
    Drum buffer[8];
    CymbalMics cymbals[5];
    float fMix;
    
};

#endif
