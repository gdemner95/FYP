#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginAudioProcessorEditor::PluginAudioProcessorEditor (PluginAudioProcessor* ownerFilter)
: AudioProcessorEditor (ownerFilter),
midiKeyboard (ownerFilter->keyboardState, MidiKeyboardComponent::horizontalKeyboard),
scope_mode(SCOPE_VISIBLE|SCOPE_SONOGRAM), oscilloscope(NULL), spectrum(NULL), sonogram(NULL), scopeThread("Scope Thread"),
tabScope(TabbedButtonBar::TabsAtTop), infoLabel (String::empty)
{
    // add controls..
    for(int c=0; c<kNumberOfControls; c++){
        
        switch(UI_CONTROLS[c].type){
            case ROTARY:
            case SLIDER:
            {   controls[c] = new Slider();
                Slider* pSlider = (Slider*)controls[c];
                
                //                addAndMakeVisible (pSlider);
                tabScope.addAndMakeVisible(pSlider);
                pSlider->setSliderStyle (UI_CONTROLS[c].type == ROTARY ? Slider::Rotary : Slider::LinearVertical);
                pSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 25, 20);
                pSlider->addListener (this);
                pSlider->setRange (UI_CONTROLS[c].min, UI_CONTROLS[c].max, 0.001);
                
                // add a label
                label[c].setText(UI_CONTROLS[c].name, dontSendNotification);
                label[c].attachToComponent(controls[c], false);
                label[c].setFont (Font (11.0f));
                label[c].setJustificationType(Justification::centredBottom);
            } break;
            case SLIDERBAR:
            {   controls[c] = new Slider();
                Slider* pSlider = (Slider*)controls[c];
                
                //                addAndMakeVisible (pSlider);
                tabScope.addAndMakeVisible(pSlider);
                pSlider->setSliderStyle (UI_CONTROLS[c].type == ROTARY ? Slider::Rotary : Slider::LinearBarVertical);
                pSlider->setTextBoxStyle (Slider::TextBoxBelow, false, 50, 20);
                pSlider->addListener (this);
                pSlider->setRange (UI_CONTROLS[c].min, UI_CONTROLS[c].max, 0.001);
                pSlider->setTextBoxIsEditable(false);
                pSlider->setEnabled(false);
                // add a label
                label[c].setText(UI_CONTROLS[c].name, dontSendNotification);
                label[c].attachToComponent(controls[c], false);
                label[c].setFont (Font (11.0f));
                label[c].setJustificationType(Justification::centred);
            } break;
                
            case TOGGLE:
            case BUTTON:
            {   controls[c] = new TextButton();
                TextButton* pButton = (TextButton*)controls[c];
                
                //                addAndMakeVisible (pButton);
                tabScope.addAndMakeVisible(pButton);
                pButton->addListener (this);
                pButton->setButtonText(UI_CONTROLS[c].name);
                pButton->setClickingTogglesState(UI_CONTROLS[c].type == TOGGLE);
            } break;
            case MENU:
            {
                controls[c] = new ComboBox();
                ComboBox* pList = (ComboBox*)controls[c];
                //                addAndMakeVisible(pList);
                tabScope.addAndMakeVisible(pList);
                for(int i=0; i<8 && UI_CONTROLS[c].options[i]; i++)
                    pList->addItem(UI_CONTROLS[c].options[i], i+1);
                pList->addListener (this);
                
                // add a label
                label[c].setText(UI_CONTROLS[c].name, dontSendNotification);
                label[c].attachToComponent(controls[c], false);
                label[c].setFont (Font (11.0f));
                label[c].setJustificationType(Justification::centred);
            }
        }
        
    }
    /*
     
     // add an oscilloscope..
     oscilloscope = new AudioOscilloscope();
     oscilloscope->setHorizontalZoom(0.001);
     oscilloscope->setTraceColour(Colours::white);
     
     spectrum = new Spectroscope(10);
     spectrum->setLogFrequencyDisplay(true);
     
     sonogram = new Sonogram(10);
     sonogram->setLogFrequencyDisplay(true);
     */

    for(int a = 0; a < 6; a++){
        for (int b = 0; b < 16; b++){
            stepSequencer[a].stepButtons[b] = new TextButton;
            TextButton* pButton = (TextButton*)stepSequencer[a].stepButtons[b];
            addAndMakeVisible(pButton);
            pButton->addListener(this);
            pButton->setClickingTogglesState(TOGGLE);
            pButton->setBounds(100 + 30 * (b + 1), 30 * (a + 1), 25, 25);
            sequenceLabel[a].setBounds(20, 30 * (b + 1), 25, 25);
            addAndMakeVisible(&sequenceLabel[a]);
            sequenceLabel[a].setFont (Font (11.0f));
            sequenceLabel[a].setText(String(a), dontSendNotification);
        }
    }

    
    addAndMakeVisible(&tabScope);
    tabScope.addTab("Mixer", Colours::grey, 0, false, 0);
    
    tabScope.addTab("Sequencer", Colours::grey, 0, false, 1);
    tabScope.setTabBarDepth(24);
    tabScope.setIndent(4);
    
    // add the midi keyboard component..
    addAndMakeVisible (&midiKeyboard);
    
    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible (resizer = new ResizableCornerComponent (this, &resizeLimits));
    resizeLimits.setSizeLimits (100, 100, 1024, 800);
    
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (  ownerFilter->lastUIWidth,
             ownerFilter->lastUIHeight);
    
    startTimer (50);
    
    midiKeyboard.grabKeyboardFocus();
}

PluginAudioProcessorEditor::~PluginAudioProcessorEditor()
{
    scope_mode = SCOPE_HIDDEN;
    
    removeChildComponent(&tabScope);
    
    scopeThread.stopThread(1000);
    stopTimer();
    
    for(int c=0; c<kNumberOfControls; c++){
        delete controls[c];
        controls[c] = NULL;
    }
    
    delete spectrum;
    spectrum = NULL;
    
    delete sonogram;
    sonogram = NULL;
    
    delete oscilloscope;
    oscilloscope = NULL;
}

void PluginAudioProcessorEditor::userTriedToCloseWindow(){
    scope_mode = SCOPE_HIDDEN;
}

//==============================================================================
void PluginAudioProcessorEditor::paint (Graphics& g)
{
    g.setGradientFill (ColourGradient (Colours::slategrey, 0, 0, Colours::slategrey, 0, (float) getHeight(), false));
    g.fillAll();
}

void PluginAudioProcessorEditor::resized()
{
    const int keyboardHeight = 100;
    
    Bounds size;
    for(int c=0; c<kNumberOfControls; c++){
        if(UI_CONTROLS[c].size == AUTO_SIZE){
            int column = c % 5;
            int row = c / 5;
            
            switch (UI_CONTROLS[c].type){
                case ROTARY:
                case SLIDER:
                    size.setBounds(20 + 75 * column, 30 + 120 * row, 50, 80);
                    break;
                case SLIDERBAR://EDIT GEORGEDEMNER 4/12/15
                    size.setBounds(20 + 75 * column, 30 + 120 * row, 50, 80);
                    break;
                case TOGGLE:
                case BUTTON:
                    size.setBounds (15 + 75 * column, 10 + 120 * row, 60, 20);
                    break;
                case MENU:
                    size.setBounds (15 + 75 * column, 30 + 120 * row, 60, 20);
                    break;
            }
        }else{
            size = UI_CONTROLS[c].size;
        }
        
        controls[c]->setBounds (size.getX(), size.getY(), size.getWidth(), size.getHeight());
        label[c].setTopLeftPosition(size.getX() - 20, size.getY() - 20);
        label[c].setSize(size.getWidth() + 40, 20);
    }
    
    tabScope.setBounds(0, 0, getWidth(), getHeight() - keyboardHeight);
    
    midiKeyboard.setBounds (4, getHeight() - keyboardHeight - 4, getWidth() - 8, keyboardHeight);
    
    resizer->setBounds (getWidth(), getHeight(), 16, 16);
    
    scope_mode = (scope_mode & ~SCOPE_VISIBLE) | (getWidth() > 400 ? SCOPE_VISIBLE : SCOPE_HIDDEN);
    
    getProcessor()->lastUIWidth = getWidth();
    getProcessor()->lastUIHeight = getHeight();
}

//==============================================================================
// This timer periodically checks whether any of the filter's parameters have changed...
void PluginAudioProcessorEditor::timerCallback()
{
    if(tabScope.getCurrentTabIndex() == 0){
        currentTab = 0;
        if (previousTab != currentTab){
            printf("Mixer");
            for (int i = 0; i < kNumberOfControls; i++){
                tabScope.addAndMakeVisible(controls[i]);
                tabScope.addAndMakeVisible(&label[i]);
            }
            for(int a = 0; a < 6; a++){
                for (int b = 0; b < 16; b++){
                    tabScope.removeChildComponent(stepSequencer[a].stepButtons[b]);
                }
            }
            previousTab = 0;
        }
      
    }
    else if (tabScope.getCurrentTabIndex() == 1){
        currentTab = 1;
        if (previousTab != currentTab){
            printf("Sequencer");
            for (int i = 0; i < kNumberOfControls; i++){
                tabScope.removeChildComponent(controls[i]);
                tabScope.removeChildComponent(&label[i]);
            }
            for(int a = 0; a < 6; a++){
                for (int b = 0; b < 16; b++){
                    tabScope.addAndMakeVisible(stepSequencer[a].stepButtons[b]);
                    
                }
            }
            previousTab = 1;
        }
       
    }
    //loop that counts 16 and resets to 0
    //check each button
    //play samples
    //
    PluginAudioProcessor* ourProcessor = getProcessor();
    
    AudioPlayHead::CurrentPositionInfo newPos (ourProcessor->lastPosInfo);
    
    if (lastDisplayedPosition != newPos)
        displayPositionInfo (newPos);
    
    for(int c=0; c<kNumberOfControls && controls[c]; c++){
        switch (UI_CONTROLS[c].type){
            case ROTARY:
            case SLIDER:
                ((Slider*)controls[c])->setValue (ourProcessor->getParameter(c), dontSendNotification);
                break;
            case SLIDERBAR://EDIT GEORGEDEMNER 4/12/15
                ((Slider*)controls[c])->setValue (ourProcessor->getParameter(c), dontSendNotification);
                break;
                
            case MENU:
                ((ComboBox*)controls[c])->setSelectedId(ourProcessor->getParameter(c)+1, dontSendNotification);
                break;
            case BUTTON:
                break;
            case TOGGLE:
                ((TextButton*)controls[c])->setToggleState(ourProcessor->getParameter(c) != 0.0, sendNotification);
                break;
        }
    }
}

// This is our Slider::Listener callback, when the user drags a slider.
void PluginAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    for(int c=0; c<kNumberOfControls; c++){
        if (slider == controls[c])
        {
            // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
            // by the host, rather than just modifying them directly, otherwise the host won't know
            // that they've changed.
            getProcessor()->setParameterNotifyingHost (c, (float) slider->getValue());
        }
    }
    
    midiKeyboard.grabKeyboardFocus();
}


void PluginAudioProcessorEditor::comboBoxChanged (ComboBox* comboBox)
{
    for(int c=0; c<kNumberOfControls; c++){
        if (comboBox == controls[c])
        {
            // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
            // by the host, rather than just modifying them directly, otherwise the host won't know
            // that they've changed.
            getProcessor()->setParameterNotifyingHost (c, (float) comboBox->getSelectedId() - 1);
        }
    }
    
    midiKeyboard.grabKeyboardFocus();
}

void PluginAudioProcessorEditor::buttonClicked(Button* button)
{
    for(int c=0; c<kNumberOfControls; c++){
        if (button == controls[c])
        {
            getProcessor()->onButtonClicked(c);
        }
    }
    
    midiKeyboard.grabKeyboardFocus();
}

void PluginAudioProcessorEditor::buttonStateChanged(Button* button)
{
    for(int c=0; c<kNumberOfControls; c++){
        if (button == controls[c])
        {
            if(UI_CONTROLS[c].type == TOGGLE){
                // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
                // by the host, rather than just modifying them directly, otherwise the host won't know
                // that they've changed.
                getProcessor()->setParameterNotifyingHost(c, button->getToggleState() ? 1.0 : 0.0);
            }
        }
    }
    
    midiKeyboard.grabKeyboardFocus();
}

// Updates the text in our position label.
void PluginAudioProcessorEditor::displayPositionInfo (const AudioPlayHead::CurrentPositionInfo& pos)
{
    lastDisplayedPosition = pos;
    String displayText;
    displayText.preallocateBytes (128);
    
    displayText << String (pos.bpm, 2) << " bpm, "
    << pos.timeSigNumerator << '/' << pos.timeSigDenominator
    << "  -  " << timeToTimecodeString (pos.timeInSeconds)
    << "  -  " << ppqToBarsBeatsString (pos.ppqPosition, pos.ppqPositionOfLastBarStart,
                                        pos.timeSigNumerator, pos.timeSigDenominator);
    
    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";
    
    infoLabel.setText (displayText, dontSendNotification);
}

  void Sequencer::addButtons(){
    for(int a = 0; a < 6; a++){
        for (int b = 0; b < 16; b++){
            piece[a].stepButtons[b] = new TextButton;
            addAndMakeVisible(piece[a].stepButtons[b]);
        }
    }
    
    
};