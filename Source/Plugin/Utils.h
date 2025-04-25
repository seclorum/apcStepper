#pragma once
#include <JuceHeader.h>

namespace Utils
{
    inline void safeGuiUpdate(const std::function<void()>& callback)
    {
        juce::MessageManager::callAsync(callback);
    }
}
