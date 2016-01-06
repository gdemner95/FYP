//
//  SynthPlugin.cpp
//  TestSynthAU
//
//  Software Synthesizer template project for UWE AMT/CMT students.
//

#include "SynthPlugin.h"

const char* fileName[19] = {
    "Bass Drum In ",
    "Bass Drum Out ",
    "Snare Up ",
    "Snare Down ",
    "High Tom ",
    "Mid Tom ",
    "Floor Tom ",
    "Hats Closed Tip ",
    "Hats Closed Shaft ",
    "Hats Rock Sizzle ",
    //    "Hats Tight Sizzle ",
    "Hats Open ",
    //    "Hats Pedal ",
    "Ride Tip ",
    "Ride Bell ",
    "Splash Crash ",
    "Crash Crash ",
    //    "Crash Bell ",
    //    "Crash Tip "
};
const char* velocityIndex[6]{
    "1_",
    "2_",
    "3_",
    "4_",
    "5_",
    "6_",
};
const char* stringEnd[6] = {
    "1.wav",
    "2.wav",
    "3.wav",
    "4.wav",
    "5.wav",
    "6.wav"
};
const char* cymbalMics[5]{
    "Close Mic ",
    "OH L ",
    "OH R ",
    "Room L ",
    "Room R "
};
/*
 ////////////////////////////////////////////////////////////////////////////
 //Currently only running the hardest samples                              //
 ////////////////////////////////////////////////////////////////////////////
 //                                                                        //
 ////////////////////////////////////////////////////////////////////////////
 // SYNTH - represents the whole synthesiser                               //
 ////////////////////////////////////////////////////////////////////////////
 */
// Called to create the synthesizer (use to point JUCE to your synthesizer)
Synth* JUCE_CALLTYPE createSynth() {
    return new MySynth();
}

// Called when the synthesiser is first created
void MySynth::initialise()
{
    // Initialise synthesiser variables here
    char charBuffer[128] = { 0 };
    for(int a = 0; a < 7; a++){
        for (int x = 5; x < 6; x++){
            for (int i = 0; i < 6; i++){
                sprintf(charBuffer, "%s%s%s", fileName[a], velocityIndex[x], stringEnd[i]);
                buffer[a].velocities[x].samples[i].openResource(charBuffer);
                buffer[a].velocities[x].samples[i].reset();
                printf("Buffer - %d Velocity - %d Sample - %d %s\n", a, x, i, charBuffer);
            }
        }
    }
    for(int b = 0; b < 8; b++){
        for(int a = 0; a < 5; a++){
            for (int x = 5; x < 6; x++){
                for (int i = 0; i < 6; i++){
                    sprintf(charBuffer, "%s%s%s%s", fileName[b+7], cymbalMics[a], velocityIndex[x], stringEnd[i]);
                    cymbals[b].mics[a].velocities[x].samples[i].openResource(charBuffer);
                    cymbals[b].mics[a].velocities[x].samples[i].reset();
                    printf("Cymbal - %d Mics - %d Velocity - %d Sample - %d %s\n", b, a, x, i, charBuffer);
                }
            }
        }
    }
    
    for(int i = 0; i < 19; i++){
        pSubmix[i] = new float[16384];
        for(int s=0; s < 16384; s++)
            pSubmix[i][s] = 0;
        //        memset(pSubmix[i], 0, sizeof(float) * 16384);
    }
    
    
    
}

// Used to apply any additional audio processing to the synthesisers' combined output
// (when called, outputBuffer contains all the voices' audio)
void MySynth::postProcess(float** outputBuffer, int numChannels, int numSamples)
{
    // Use to add global effects, etc.
    float fDry[19];
    
    float* pMainOutput0 = outputBuffer[0];
    float* pMainOutput1 = outputBuffer[1];
    
    float fLeftBuffer[19];
    float fRightBuffer[19];
    
    float* pfSubmix[19] = {
        pSubmix[0],
        pSubmix[1],
        pSubmix[2],
        pSubmix[3],
        pSubmix[4],
        pSubmix[5],
        pSubmix[6],
        pSubmix[7],
        pSubmix[8],
        pSubmix[9],
        pSubmix[10],
        pSubmix[11],
        pSubmix[12],
        pSubmix[13],
        pSubmix[14],
        pSubmix[15],
        pSubmix[16],
        pSubmix[17],
        pSubmix[18],
    };
    
    float fLevel[8];
    float fPanner[8];
    float dbFs[8] = { 0 };
    
    for(int i = 0; i < 7; i++){
        fPanner[i] = getParameter(kParam16+i);
        fLevel[i] = getParameter(kParam0+i);
    }
    for(int i = 0; i < 7; i++){
        dbFs[i] = fabsf(*pfSubmix[i]);
        setParameter(kParam8+i, dbFs[i]);
    }
    for(int i = 7; i < 16; i++){
        dbFs[7] += fabsf(*pfSubmix[i]);
        setParameter(kParam8+i, dbFs[i]);
    }
    
    while(numSamples--)
    {
        for(int i = 0; i < 16; i++){
            fDry[i] = *pfSubmix[i];
        }
        // Add your global effect processing here
        //apply panning
        for(int i = 0; i < 7; i++){
            fLeftBuffer[i] = ((fDry[i] * fLevel[i]) * (1.0 - fPanner[i]));
            fRightBuffer[i] = ((fDry[i] * fLevel[i]) * fPanner[i]);
        };
        for(int i = 7; i < 16; i++){
            fLeftBuffer[i] = fDry[i] * 0.5;
            fRightBuffer[i] = fDry[i] * 0.5;
        };
        
        *pMainOutput0++ += fLeftBuffer[0] + fLeftBuffer[1] + fLeftBuffer[2] + fLeftBuffer[3] + fLeftBuffer[4] + fLeftBuffer[5] + fLeftBuffer[6] + fLeftBuffer[7] + fLeftBuffer[8] + fLeftBuffer[9] + fLeftBuffer[10] + fLeftBuffer[11] + fLeftBuffer[12] + fLeftBuffer[13] + fLeftBuffer[14] + fLeftBuffer[15];
        *pMainOutput1++ += fRightBuffer[0] + fRightBuffer[1] + fRightBuffer[2] + fRightBuffer[3] + fLeftBuffer[4] + fLeftBuffer[5] + fLeftBuffer[6] + fLeftBuffer[7] + fLeftBuffer[8] + fLeftBuffer[9] + fRightBuffer[10] + fRightBuffer[11] + fRightBuffer[12] + fRightBuffer[13] + fRightBuffer[14] + fRightBuffer[15];
        
        for(int i = 0; i < 16; i++){
            *pfSubmix[i]++ = 0;
        }
        
    }
}

////////////////////////////////////////////////////////////////////////////
// VOICE - represents a single note in the synthesiser
////////////////////////////////////////////////////////////////////////////

// Called to create the voice (use to point JUCE to your voice)
Voice* JUCE_CALLTYPE createVoice() {
    return new MyVoice();
}

// Triggered when a note is started (use to initialise / prepare note)
void MyVoice::onStartNote (const int pitch, const float velocity)
{
    printf("Note start\n");
    
    this->pitch = pitch;
    //bass drum
    if (pitch == 48){
        signalGenerator[0] = *getSynthesiser()->getBuffer(0,velocity);
        signalGenerator[1] = *getSynthesiser()->getBuffer(1,velocity);
    }
    //snare
    else if (pitch == 50){
        signalGenerator[0] = *getSynthesiser()->getBuffer(2,velocity);
        signalGenerator[1] = *getSynthesiser()->getBuffer(3,velocity);
    }
    //hats closed
    else if (pitch == 54){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(0, velocity, i);
        }
        
    }
    //hats Rock sizzle
    else if (pitch == 56){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(2, velocity, i);
        }
    }
    //openHats
    else if (pitch == 58){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(3, velocity, i);
        }
    }
    //Crash crash
    else if (pitch == 60){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(7, velocity, i);
        }
    }
    //crash bell
    
    //crash tip
    
    //ride tip
    else if (pitch == 63){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(4, velocity, i);
        }
    }
    //ride crash
    
    //ride bell
    else if (pitch == 65){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(5, velocity, i);
        }
    }
    //splash crash
    else if (pitch == 66){
        for(int i = 0; i < 5; i++){
            signalGenerator[i] = *getSynthesiser()->getCymbalBuffer(6, velocity, i);
        }
    }
    //high tom
    else if (pitch == 57){
        signalGenerator[0] = *getSynthesiser()->getBuffer(4,velocity);
    }
    //mid tom
    else if (pitch == 55){
        signalGenerator[0] = *getSynthesiser()->getBuffer(5,velocity);
    }
    //floor tom
    else if (pitch == 53){
        signalGenerator[0] = *getSynthesiser()->getBuffer(6,velocity);
    }
    
    else
    {
//        for(int i = 0; i < 8; i++){
//        signalGenerator[i].reset();
//        }
    }
    
    fLevel = velocity;
    iSilenceCount = 0;
}

// Triggered when a note is stopped (return false to keep the note alive)
bool MyVoice::onStopNote (){
    
    //    noteOffEnv.release(0.5);
    return false;
}

// Called to render the note's next buffer of audio (generates the sound)
// (return false to terminate the note)
bool MyVoice::process (float** outputBuffer, int numChannels, int numSamples)
{
    float* pfOutBuffer0 = outputBuffer[0];
    float* pfOutBuffer1 = outputBuffer[1];
    float** pfSubmixes = getSynthesiser()->pSubmix;
    float* pfSubmix[19] = {  pfSubmixes[0], pfSubmixes[1], pfSubmixes[2], pfSubmixes[3], pfSubmixes[4], pfSubmixes[5], pfSubmixes[6],  pfSubmixes[7],  pfSubmixes[8],  pfSubmixes[9],  pfSubmixes[10],  pfSubmixes[11],  pfSubmixes[12],  pfSubmixes[13], pfSubmixes[14],  pfSubmixes[15],  pfSubmixes[16],  pfSubmixes[17],  pfSubmixes[19]};
    
    while(numSamples--)
    {
        if (pitch == 48)
        {
            *pfSubmix[0] += signalGenerator[0].tick();
            *pfSubmix[1] += signalGenerator[1].tick();
        }
        else if (pitch == 50){
            *pfSubmix[2] += signalGenerator[0].tick();
            *pfSubmix[3] += signalGenerator[1].tick();
        }
        //hats closed tip
        else if (pitch == 54){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
//            *pfSubmix[7] += signalGenerator[0].tick();
//            *pfSubmix[8] += signalGenerator[1].tick();
//            *pfSubmix[9] += signalGenerator[2].tick();
//            *pfSubmix[10] += signalGenerator[3].tick();
//            *pfSubmix[11] += signalGenerator[4].tick();
        }
        //hats sizzle
        else if (pitch == 56){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
        }
        //hats open
        else if (pitch == 58){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
        }
        //ride tip
        else if (pitch == 63){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
        }
        //ride bell
        else if (pitch == 65){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
        }
        //splash crash
        else if (pitch == 66){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
        }
        //crash crash
        else if (pitch == 60){
            for(int i = 7; i < 12; i++){
                *pfSubmix[i] += signalGenerator[i - 7].tick();
            }
        }

        else if (pitch == 53){
            *pfSubmix[4] += signalGenerator[0].tick();
        }
        else if (pitch == 55){
            *pfSubmix[5] += signalGenerator[0].tick();
        }
        else if (pitch == 57){
            *pfSubmix[6] += signalGenerator[0].tick();
        }
        
        for(int i = 0; i < 15; i++){
            pfSubmix[i]++;
        }
        
        *pfOutBuffer0++ = 0;
        *pfOutBuffer1++ = 0;
    }
    
    bool bActuallyEnding = !signalGenerator[0].isFinished() || !signalGenerator[1].isFinished();
    if(!bActuallyEnding)
        printf("Note terminated.\n");
    return bActuallyEnding;
}
