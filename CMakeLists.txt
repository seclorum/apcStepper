cmake_minimum_required(VERSION 3.15)

# Get the current Git commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest Git tag
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Check the hostname and set AAX_SDK_PATH if it matches "abc-jv.local"
execute_process(
		COMMAND uname -n
		OUTPUT_VARIABLE HOSTNAME
		OUTPUT_STRIP_TRAILING_WHITESPACE
)




project(apcStepper VERSION 1.0)

# JUCE Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(JUCE_ENABLE_LV2 FALSE)
set(JUCE_ENABLE_UNITY FALSE)

include(FetchContent)

FetchContent_Declare(
		tracktion_engine
		SOURCE_DIR ${CMAKE_SOURCE_DIR}/modules/tracktion_engine
		# Point to local module
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
