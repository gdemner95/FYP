//
//  SynthEditor.h
//  TestSynthAU
//
//  Used to specify the contents and layout of the TestSynthAU User Interface (UI).
//

#ifndef __SynthEditor_h__
#define __SynthEditor_h__

enum CONTROL_TYPE
{
    ROTARY, // rotary knob (pot)
    BUTTON, // push button (trigger)
    TOGGLE, // on/off switch (toggle)
    SLIDER, // linear slider (fader)
    SLIDERBAR,//linear slider (meter) EDIT::GEORGEDEMNER 4/12/15
    MENU,   // drop-down list (menu)
};

typedef Rectangle<int> Bounds;

struct Control
{
    String name;            // name for control label / saved parameter
    int parameter;          // parameter index associated with control
    CONTROL_TYPE type;      // control type (see above)
    
    // ROTARY and SLIDER only:
    float min;              // minimum slider value (e.g. 0.0)
    float max;              // maximum slider value (e.g. 1.0)
    
    float initial;          // initial value for slider (e.g. 0.0)
    
    Bounds size;            // position and size {x, y, height, width} of the control (use AUTO_SIZE for automatic layout)
    
    const char* const options[8]; // text options for menus and group buttons { "Option 0", "Option 1", ... }
};

const Bounds AUTO_SIZE = Bounds(-1,-1,-1,-1); // used to trigger automatic layout
enum { kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23 };

//=========================================================================
// UI_CONTROLS - Use this array to completely specify your UI
// - tell the system what controls you want
// - add or remove controls by adding or taking away from the list
// - each control is linked to the specified parameter name and identifier
// - controls can be of different types - rotary, button, toggle, slider (see above)
// - for rotary and linear sliders, you can set the range of values
// - by default, the controls are laid out in a grid, but you can also move and size them manually
//   i.e. replace AUTO_SIZE with Bounds(50,50,100,100) to place a 100x100 control at (50,50)
// - for MENU (drop down list) controls, enter the items in the list by adding an additional entry
//   e.g.  { "Param 0", kParam0, ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE, { "One", "Two" }  },

const Control UI_CONTROLS[] = {
    //      name,       parameter,  type,   min, max, initial,  size,      options (for MENU)
    {   "Kick In",      kParam0,    SLIDER, 0.0, 1.3, 1.0,           Bounds(0,   30, 50, 180)   },
    {   "Kick Out",     kParam1,    SLIDER, 0.0, 1.3, 1.0,           Bounds(80,  30, 50, 180)   },
    {   "Snare Up",     kParam2,    SLIDER, 0.0, 1.3, 1.0,           Bounds(160, 30, 50, 180)   },
    {   "Snare Down",   kParam3,    SLIDER, 0.0, 1.3, 1.0,           Bounds(240, 30, 50, 180)   },
    {   "High Tom",     kParam4,    SLIDER, 0.0, 1.3, 1.0,           Bounds(320, 30, 50, 180)   },
    {   "Mid Tom",      kParam5,    SLIDER, 0.0, 1.3, 1.0,           Bounds(400,   30, 50, 180)   },
    {   "Floor Tom",     kParam6,    SLIDER, 0.0, 1.3, 1.0,           Bounds(480,  30, 50, 180)   },
    {   "OverHeads",     kParam7,    SLIDER, 0.0, 1.3, 1.0,           Bounds(560, 30, 50, 180)   },
    
    //VU Meters
    {   "",      kParam8,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(40,  30, 10, 180)   },
    {   "",      kParam9,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(120, 30, 10, 180)   },
    {   "",      kParam10,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(200, 30, 10, 180)   },
    {   "",      kParam11,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(280, 30, 10, 180)   },
    {   "",      kParam12,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(360, 30, 10, 180)   },
    {   "",      kParam13,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(400, 30, 10, 180)   },
    {   "",      kParam14,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(480, 30, 10, 180)   },
    {   "",      kParam15,    SLIDERBAR, 0.0, 1.0, 0.0,       Bounds(560, 30, 10, 180)   },
    //Panners
    {   "",      kParam16,   ROTARY, 0.0, 1.0, 0.5,          Bounds(50,  30, 40, 40)   },
    {   "",      kParam17,   ROTARY, 0.0, 1.0, 0.5,          Bounds(130, 30, 40, 40)   },
    {   "",      kParam18,   ROTARY, 0.0, 1.0, 0.5,          Bounds(210, 30, 40, 40)   },
    {   "",      kParam19,   ROTARY, 0.0, 1.0, 0.5,          Bounds(290, 30, 40, 40)   },
    {   "",      kParam20,   ROTARY, 0.0, 1.0, 0.5,          Bounds(370, 30, 40, 40)   },
    {   "",      kParam21,   ROTARY, 0.0, 1.0, 0.5,          Bounds(450, 30, 40, 40)   },
    {   "",      kParam22,   ROTARY, 0.0, 1.0, 0.5,          Bounds(530, 30, 40, 40)   },
    {   "",      kParam23,   ROTARY, 0.0, 1.0, 0.5,          Bounds(610, 30, 40, 40)   },
//    {   "Pan",          kParam15,   ROTARY, 0.0, 1.0, 0.5,          Bounds(450, 10, 40, 40)   },
//    {   "Pan",          kParam16,   ROTARY, 0.0, 1.0, 0.5,          Bounds(530, 10, 40, 40)   },
//    {   "Pan",          kParam17,   ROTARY, 0.0, 1.0, 0.5,          Bounds(610, 10, 40, 40)   },
};

const int kNumberOfControls = sizeof(UI_CONTROLS) / sizeof(Control);
const int kNumberOfParameters = kNumberOfControls;

#endif
