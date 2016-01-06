#ifndef __PLUGINEDITOR_H__
#define __PLUGINEDITOR_H__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "SynthEditor.h"

using namespace drow;

enum UI_MODE
{
    SCOPE_HIDDEN = 0,
    SCOPE_VISIBLE = 1,
    SCOPE_MIXER = 2,
    SCOPE_SEQUENCER = 4,
    SCOPE_SONOGRAM = 8
};
struct drumSequencer{
    Button* stepButtons[16];
    
};
//==============================================================================
/** This is the editor component that our filter will display.
*/
class PluginAudioProcessorEditor  : public AudioProcessorEditor,
                                            public SliderListener,
                                            public ButtonListener,
                                            public ComboBoxListener,
                                            public Timer
{
    friend class PluginAudioProcessor;
public:
    PluginAudioProcessorEditor (PluginAudioProcessor* ownerFilter);
    ~PluginAudioProcessorEditor();

    //==============================================================================
    void timerCallback();
    void paint (Graphics& g);
    void resized();
    
    void userTriedToCloseWindow();
    
    void sliderValueChanged (Slider*);
    void buttonClicked(Button*);
    void buttonStateChanged(Button* button);
    void comboBoxChanged (ComboBox* comboBox);

private:
    MidiKeyboardComponent midiKeyboard;
    
    int currentTab;
    int previousTab;
    int scope_mode;
    
    drumSequencer stepSequencer[6];
    Label sequenceLabel[6];
    AudioOscilloscope *oscilloscope;
    Spectroscope *spectrum;
    Sonogram *sonogram;
    
    TimeSliceThread scopeThread;
    
    TabbedComponent tabScope;
    
    Label infoLabel;
    
    Label label[kNumberOfControls];
    Component* controls[kNumberOfControls];
    Component* mixer;
    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;

    AudioPlayHead::CurrentPositionInfo lastDisplayedPosition;

    PluginAudioProcessor* getProcessor() const
    {
        return static_cast <PluginAudioProcessor*> (getAudioProcessor());
    }

    void displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos);
};


class Sequencer : public AudioProcessorEditor,
                public SliderListener,
                public ButtonListener,
                public ComboBoxListener,
                public Timer
{
public:
   virtual void addButtons();
    
private:
    drumSequencer piece[6];
    
    Component* controls[kNumberOfControls];
    Label infoLabel;
    Label label[kNumberOfControls];
    Component* mixer;
    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;
};
#endif  // __PLUGINEDITOR_H_4ACCBAA__
