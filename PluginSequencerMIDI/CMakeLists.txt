cmake_minimum_required(VERSION 3.15)

set(VERBOSE)

project(PluginSequencerMIDI VERSION 1.0.0)

add_subdirectory(modules/juce)
add_subdirectory(modules/)

set(FORMATS Standalone AU VST3 AUv3)

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
	target_link_options(PluginSequencerMIDI INTERFACE -Wl )
	set(CMAKE_OSX_ARCHITECTURES arm64 x86_64 )
#	set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "macOS deployment target")
	#	if(CMAKE_OSX_DEPLOYMENT_TARGET)
	#		message("The minimum macOS version is set to " $CACHE{CMAKE_OSX_DEPLOYMENT_TARGET}.)
	#	endif()
#	set(CMAKE_MACOSX_RPATH ON)
#	message(STATUS "macOS RPATH enabled: ${CMAKE_MACOSX_RPATH}")
	# set(CMAKE_XCODE_GENERATE_SCHEME “TRUE”)
	# set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum version of the target platform" FORCE)
else ()
	message("Building on Windows")
	set(FORMATS VST3)
endif ()

target_sources(PluginSequencerMIDI PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessor.h
		${CMAKE_CURRENT_SOURCE_DIR}/Source/PluginProcessorEditor.h
)


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


