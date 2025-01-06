cmake_minimum_required(VERSION 3.15)

project(PluginSequecerMIDI VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FetchContent for JUCE and Tracktion Engine
include(FetchContent)

# Fetch JUCE
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.3 # Replace with the desired JUCE version
)
FetchContent_MakeAvailable(JUCE)

# Fetch Tracktion Engine
FetchContent_Declare(
    tracktion_engine
    GIT_REPOSITORY https://github.com/Tracktion/tracktion_engine.git
    GIT_TAG        develop # Replace with a stable release if needed
)
FetchContent_MakeAvailable(tracktion_engine)

# Add the plugin target
juce_add_plugin(
    PluginSequecerMIDI
    COMPANY_NAME "YourCompanyName"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Demo
)

# Add source files
target_sources(PluginSequecerMIDI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.h
)

# Include JUCE and Tracktion Engine
target_link_libraries(PluginSequecerMIDI PRIVATE
    juce::juce_audio_utils
    tracktion_engine
)

# Set the JUCE configuration
juce_generate_juce_header(PluginSequecerMIDI)

# Copy the built plugin to the appropriate location
set(COPY_DESTINATION ${CMAKE_BINARY_DIR}/Plugins)
set_target_properties(PluginSequecerMIDI PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${COPY_DESTINATION})

