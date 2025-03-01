set(VERBOSE)

cmake_minimum_required(VERSION 3.15)

# JUCE Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(JUCE_ENABLE_LV2 FALSE)
set(JUCE_ENABLE_UNITY FALSE)

# Add JUCE modules
add_subdirectory(modules/juce)
add_subdirectory(modules/melatonin_inspector)

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/Assets")

# Define the destination directory in the build folder
set(ASSETS_DEST_DEBUG "${CMAKE_BINARY_DIR}/apcSequencer_artefacts/Debug/Standalone/Assets")
set(ASSETS_DEST_RELEASE "${CMAKE_BINARY_DIR}/apcSequencer_artefacts/Release/Standalone/Assets")




# Message to verify source directory
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Get the current Git commit hash (avoid errors if not in Git repo)
execute_process(
		COMMAND git rev-parse --short HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_COMMIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
)

# Get the latest Git tag (avoid errors if not in Git repo)
execute_process(
		COMMAND git describe --tags --abbrev=0
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_TAG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
)

# Check the hostname and set AAX_SDK_PATH if needed
execute_process(
		COMMAND uname -n
		OUTPUT_VARIABLE HOSTNAME
		OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Create a single variable for shared JUCE modules
set(JUCE_MODULES
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
		melatonin_inspector
)

# ==============================================================================
# APPLICATION: apcStepper
# ==============================================================================
project(apcStepper VERSION 1.0)

juce_add_gui_app(
		apcStepper
		PRODUCT_NAME "apcStepper"
)

# Automatically add all application source files
file(GLOB_RECURSE APP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.h)

target_sources(apcStepper PRIVATE ${APP_SOURCE_FILES})

# Link JUCE modules
target_link_libraries(apcStepper PRIVATE ${JUCE_MODULES})

# ==============================================================================
# PLUGIN: apcSequencer
# ==============================================================================
project(apcSequencer VERSION 1.0.0)

set(FORMATS Standalone AU VST3 AUv3)



juce_add_plugin(
		apcSequencer
		COMPANY_NAME "enabled.io"
		IS_SYNTH FALSE
		NEEDS_MIDI_INPUT TRUE
		NEEDS_MIDI_OUTPUT TRUE
		IS_MIDI_EFFECT TRUE
		EDITOR_WANTS_KEYBOARD_FOCUS TRUE
		COPY_PLUGIN_AFTER_BUILD ON
		PLUGIN_MANUFACTURER_CODE ENIO
		PLUGIN_CODE SEQ1
		FORMATS ${FORMATS}
)


# Automatically add all plugin source files
file(GLOB_RECURSE PLUGIN_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.h)

target_sources(apcSequencer PRIVATE ${PLUGIN_SOURCE_FILES})

# Set plugin compile definitions
target_compile_definitions(apcSequencer
		INTERFACE
		JUCE_WEB_BROWSER=0  # If enabled, add `NEEDS_WEB_BROWSER TRUE` to `juce_add_plugin`
		JUCE_USE_CURL=0     # If enabled, add `NEEDS_CURL TRUE` to `juce_add_plugin`
		JUCE_VST3_CAN_REPLACE_VST2=0
		JUCE_MODAL_LOOPS_PERMITTED=1
		CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
		VERSION="${PROJECT_VERSION}"
		BUILD_COMMIT_HASH="${GIT_COMMIT_HASH}"
		BUILD_TAG="${GIT_TAG}"
		PRODUCT_NAME_WITHOUT_VERSION="apcStepper"
)


juce_add_binary_data(Assets
		SOURCES
		Assets/cuttlefish.jpg
		Assets/shadow.png
)


# Link JUCE modules
target_link_libraries(apcSequencer PRIVATE ${JUCE_MODULES} Assets)



# Set macOS architectures for universal binary (Apple Silicon + Intel)
if (APPLE)
	message("Building on macOS (Darwin)")
	target_link_options(apcSequencer INTERFACE -Wl)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build for Apple Silicon and Intel")
endif()
