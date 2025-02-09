//
// Created by Tom Peak Walcher on 09.02.25.
//

#include "PluginProcessor.h"

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new apcSequencerProcessor();
};


