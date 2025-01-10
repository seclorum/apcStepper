/*
  ==============================================================================

    This file was auto-generated and contains the startup code for a PIP.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/examples/EngineInPluginDemo/EngineInPluginDemo.h"

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EngineInPluginDemo();
}
