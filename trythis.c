/* 

    whip this shit in your loop and see if it does bits
    if its shit I'll have another look at it
    don't the first 4 bits need to go up top as they're calling Arduino.h and AudioStream.h from the man above
    lmk if it cuts off more frequencies top or bottom

    line 31 includes a signed 16-bit integer variable by the name 'thresholdLevelInt', which is equal to the product of two variables, x and 32767.0f (the maximum threshold). Edit the value of x so that the product equals the decibel cutoff you want.

*/


#ifndef effect_gate_h_ // check if effect_gate_h_ is not defined
#define effect_gate_h_ // define effect_gate_h_

#include "Arduino.h"     // felt right
#include "AudioStream.h" // has functions & structures i need

const int S_Floor = 0;   // constant for the floor state (min gain when noise gate is closed)
const int S_Attack = 1;  // constant for the attack state (goes from closed state to an ascending state)
const int S_Hold = 2;    // constant fopr the hold state (audio is maintained)
const int S_Release = 3; // constant for release state (audio is descending)

class AudioEffectGate : public AudioStream // define the AudioEffectGate class as a subclass of "AudioStream"
{
public:
    AudioEffectGate(void) // constructor for AudioEffectGate
        : AudioStream(1, inputQueueArray) // initializes AudioStream with 1 input channel and inputQueueArray (might not work mind you as there may be channel interpreting confusion, if it fucks up try changing to channel -1, which is essentially directing the sample input to *all channels*)
    {
        currentGain = 1.0f; // set currentGain to 1.0 (maximum volume) initially
        thresholdLevelInt = 0.1236f * 32767.0f; //  so i set the threshold level to represent -3.65dB (0.1236f), change it to whatever you want using the formula [x * 32767.0f = n dB], solving for x as [x = (n dB) / 326767.0f] (n value being desired gate threshold)
        attackTimeDelta = 1.0f / (50.0f / (1000.0f / (AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SAMPLES)));// calculate attack time delta
        holdTimeDelta = 1.0f / (200.0f / (1000.0f / (AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SAMPLES))); // calculate hold time delta
        currentHoldTime = 0.0f;// initialize currentHoldTime to 0.0
        releaseTimeDelta = 1.0f / (200.0f / (1000.0f / (AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SAMPLES))); // now calculate the release time delta
        floorGain = 0.0f; // set floorGain to 0.0 (minimum volume)
        state = S_Floor; // set the initial state to the floor state
    }
    void threshold(float threshold) //function to set the threshold level
    {
        if (threshold > 1.0f)// so if the threshold is greater than 1.0
            threshold = 1.0f; // then pls set the threshold to 1.0
        else if (threshold < 0.0f) // however, if the threshold is less than 0.0
            threshold = 0.0f; // then can u set the threshold to 0.0
        thresholdLevelInt = threshold * 32767.0f; // ok gud now spit me out the threshold level
    }

    void attack(float milliseconds) // function to set the attack time
    {
        if (milliseconds <= 0.0f) // if the attack time is less than or equal to 0.0
        {
            milliseconds = 0.01f; // then can u set the attack time to 0.01
        }
        attackTimeDelta = 1.0f / (milliseconds / (1000.0f / (AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SAMPLES))); // & calculate the attack time delta
    }

    void hold(float milliseconds) // function to set the hold time
    {
        if (milliseconds <= 0.0f) // if the hold time is less than or equal to 0.0
        {
            milliseconds = 0.01f; // then pls set the hold time to 0.01
        }
        holdTimeDelta = 1.0f / (milliseconds / (1000.0f / (AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SAMPLES))); // calculate me the hold time delta
    }

    void release(float milliseconds) // function to set the release time
    {
        if (milliseconds <= 0.0f) // if the release time is less than or equal to 0.0
        {
            milliseconds = 0.01f; // set the release time to 0.01
        }
        releaseTimeDelta = 1.0f / (milliseconds / (1000.0f / (AUDIO_SAMPLE_RATE / AUDIO_BLOCK_SAMPLES))); // calculate the release time delta
    }

    void floor(float gain) // function to set the floor gain
    {
        if (gain < 0.0f)  // if the gain is less than 0.0
            gain = 0.0f;  // set the gain to 0.0
        if (gain > 1.0f)  // if the gain is greater than 1.0
            gain = 1.0f;  // set the gain to 1.0
        floorGain = gain; // set the floor gain
    }

    using AudioStream::release; // allow access to the release function from the base class
    virtual void update(void);  // override the update function from the base class

private:
    audio_block_t *inputQueueArray[1]; // array asking the audio blocks to politely wait in the input queue
    int16_t thresholdLevelInt;         // threshold level for the noise gate
    float attackTimeDelta;             // time delta for attack state
    float holdTimeDelta;               // time delta for hold state
    float releaseTimeDelta;            // time delta for release state
    float floorGain;                   // gain applied when the noise gate is closed
    int32_t state;                     // current state of the noise gate
    // Transient variables
    float currentGain;     // current gain applied to the audio signal
    float currentHoldTime; // current hold time for the noise gate
};

#endif // end term constructor, basically closes from #ifndef, keeping this part of the code internalised (^;