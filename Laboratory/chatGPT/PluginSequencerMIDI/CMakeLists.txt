cmake_minimum_required(VERSION 3.15)

project(PluginSequecerMIDI VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 17)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 7.0.3
)
FetchContent_MakeAvailable(JUCE)

FetchContent_Declare(
        tracktion_engine
        GIT_REPOSITORY https://github.com/Tracktion/tracktion_engine.git
        GIT_TAG develop
)
FetchContent_MakeAvailable(tracktion_engine)

juce_add_plugin(
        PluginSequecerMIDI
        COMPANY_NAME "enabled.io"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        IS_MIDI_EFFECT TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE ENIO
        PLUGIN_CODE SEQ1
        FORMATS AU VST3 Standalone
)

target_sources(PluginSequecerMIDI PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.h
)

target_link_libraries(PluginSequecerMIDI PRIVATE
        juce::juce_gui_basics
        juce::juce_audio_utils
        tracktion_engine
)
