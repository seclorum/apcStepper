cmake_minimum_required(VERSION 3.15)

project(PluginSequencerMIDI VERSION 1.0.0)
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
        PluginSequencerMIDI
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


# Sets the minimum macOS version
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	message("Building on MacOS (Darwin)")
    set(CMAKE_XCODE_GENERATE_SCHEME “TRUE”)
	# Configures universal binaries in CI
    set(CMAKE_OSX_ARCHITECTURES arm64 x86_64 )
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum version of the target platform" FORCE)
	target_link_options(PluginSequencerMIDI INTERFACE -Wl )
	if(CMAKE_OSX_DEPLOYMENT_TARGET)
		message("The minimum macOS version is set to " $CACHE{CMAKE_OSX_DEPLOYMENT_TARGET}.)
	endif()
else ()
	message("Building on Windows")
	set(FORMATS VST3)
endif ()


# C++20, please
# Use cxx_std_23 for C++23 (as of CMake v 3.20)
target_compile_features(PluginSequencerMIDI INTERFACE cxx_std_20)


# This is where you can set preprocessor definitions for JUCE and your plugin
target_compile_definitions(PluginSequencerMIDI
    INTERFACE

    # JUCE_WEB_BROWSER and JUCE_USE_CURL off by default
    JUCE_WEB_BROWSER=0  # If you set this to 1, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you set this to 1, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_MODAL_LOOPS_PERMITTED=1

    # Uncomment if you are paying for a an Indie/Pro license or releasing under GPLv3
    # JUCE_DISPLAY_SPLASH_SCREEN=0

    # lets the app known if we're Debug or Release
    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    # Add compile definitions to pass the Git hash and tag as preprocessor macros
    BUILD_COMMIT_HASH="${GIT_COMMIT_HASH}"
    BUILD_TAG="${GIT_TAG}"

	# Enable this to profile
	# PERFETTO=1

    # JucePlugin_Name is for some reason doesn't use the nicer PRODUCT_NAME
    PRODUCT_NAME_WITHOUT_VERSION="apcStepper"
)




target_link_libraries(PluginSequencerMIDI PRIVATE
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


target_sources(PluginSequencerMIDI PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginEditor.h
)
