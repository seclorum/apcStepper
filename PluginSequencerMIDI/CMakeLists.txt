cmake_minimum_required(VERSION 3.15)

project(PluginSequecerMIDI VERSION 1.0.0)
set(VERBOSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64e" CACHE STRING "macOS architectures")
message(STATUS "Target architectures: ${CMAKE_OSX_ARCHITECTURES}")
# Set deployment target (e.g., macOS 11.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "macOS deployment target")
message(STATUS "Deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
# Optionally, set additional macOS-specific flags
set(CMAKE_MACOSX_RPATH ON)
message(STATUS "macOS RPATH enabled: ${CMAKE_MACOSX_RPATH}")

include(FetchContent)
include(FetchContent)

FetchContent_Declare(
        tracktion_engine
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/../modules/tracktion_engine
        # Point to local module
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

target_link_libraries(PluginSequecerMIDI PRIVATE
        tracktion::tracktion_core
        tracktion::tracktion_engine
        tracktion::tracktion_graph
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)


target_sources(PluginSequecerMIDI PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.h
)
