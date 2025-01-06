cmake_minimum_required(VERSION 3.15)

project(apcStepper VERSION 1.0)

# JUCE Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Explicitly exclude LV2 build product
set(JUCE_ENABLE_LV2 FALSE)

# !J! modules/tracktion_engine should be a git submodule, already checked-out
# .. otherwise use either of these:
# option A: Declare local repo, i.e. add_subdirectory(modules/tracktion_engine)
# option B: Fetch the tracktion_engine (which includes JUCE) library
include(FetchContent)
FetchContent_Declare(
    tracktion_engine
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/modules/tracktion_engine # Point to the git submodule
)
FetchContent_Declare(
  tracktion_engine 
  GIT_REPOSITORY git@github.com:Tracktion/tracktion_engine.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(tracktion_engine)


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

# Link tracktion_engine module
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
