cmake_minimum_required(VERSION 3.15)

project(apcStepper VERSION 1.0)

# JUCE Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# !J! modules/JUCE should be a git submodule, already checked-out
add_subdirectory(modules/tracktion_engine)
# .. otherwise use either of these:
# Declare the JUCE library
#include(FetchContent)
#FetchContent_Declare(
#    juce
#    SOURCE_DIR ${CMAKE_SOURCE_DIR}/modules/JUCE # Point to the git submodule
#)
#FetchContent_Declare(
#  juce
#  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
#  GIT_TAG        master
#)
#FetchContent_MakeAvailable(juce)


# Add executable target
juce_add_gui_app(
  apcStepper
  PRODUCT_NAME "apcStepper"
)

# Add source files
target_sources(apcStepper
  PRIVATE
    Source/Main.cpp
)

# Link JUCE module
target_link_libraries(apcStepper
  PRIVATE
	juce_audio_basics
	juce_audio_devices
	juce_audio_formats
	juce_audio_processors
	juce_audio_utils
	juce_core
	juce_dsp
	juce_events
	juce_graphics
	juce_gui_basics
	juce_gui_extra
	juce_opengl
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
