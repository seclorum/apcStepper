# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.15)

# Project setup
project(trackDeck VERSION 1.0.0 LANGUAGES CXX)

# JUCE and Tracktion Engine setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Fetch JUCE from develop branch
include(FetchContent)
FetchContent_Declare(
		juce
		GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
		GIT_TAG origin/develop
		SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/juce
)
FetchContent_MakeAvailable(juce)

# Fetch Tracktion Engine from develop branch, excluding its JUCE
FetchContent_Declare(
		tracktion_engine
		GIT_REPOSITORY https://github.com/Tracktion/tracktion_engine.git
		GIT_TAG origin/develop
		SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/tracktion_engine
		SOURCE_SUBDIR modules
)
FetchContent_MakeAvailable(tracktion_engine)

# Include Melatonin Inspector for Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
	FetchContent_Declare(
			melatonin_inspector
			GIT_REPOSITORY https://github.com/sudara/melatonin_inspector.git
			GIT_TAG origin/main
			SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector
	)
	FetchContent_MakeAvailable(melatonin_inspector)
endif()

# Define asset directories
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Assets")
set(ASSETS_DEST_DEBUG "${CMAKE_BINARY_DIR}/trackDeck_artefacts/Debug/Standalone/Assets")
set(ASSETS_DEST_RELEASE "${CMAKE_BINARY_DIR}/trackDeck_artefacts/Release/Standalone/Assets")

# Copy assets to build directory
file(MAKE_DIRECTORY ${ASSETS_DEST_DEBUG} ${ASSETS_DEST_RELEASE})
file(COPY ${ASSETS_SOURCE_DIR}/ DESTINATION ${ASSETS_DEST_DEBUG})
file(COPY ${ASSETS_SOURCE_DIR}/ DESTINATION ${ASSETS_DEST_RELEASE})

# Get Git commit hash
execute_process(
		COMMAND git rev-parse --short HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_COMMIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
		RESULT_VARIABLE GIT_HASH_RESULT
		ERROR_QUIET
)
if(NOT GIT_HASH_RESULT EQUAL 0)
	set(GIT_COMMIT_HASH "unknown")
endif()

# Get latest Git tag
execute_process(
		COMMAND git describe --tags --abbrev=0
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_TAG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		RESULT_VARIABLE GIT_TAG_RESULT
		ERROR_QUIET
)
if(NOT GIT_TAG_RESULT EQUAL 0)
	set(GIT_TAG "unknown")
endif()

# Define JUCE modules
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
		juce::juce_audio_plugin_client
		juce::juce_recommended_config_flags
		juce::juce_recommended_lto_flags
		juce::juce_recommended_warning_flags
)

# Define Tracktion Engine modules
set(TRACKTION_MODULES
		tracktion::tracktion_engine
)

# Add binary data for assets
juce_add_binary_data(Assets
		SOURCES
		${ASSETS_SOURCE_DIR}/back.svg
		${ASSETS_SOURCE_DIR}/Conthrax-SemiBold.otf
		${ASSETS_SOURCE_DIR}/cuttlefish.jpg
		${ASSETS_SOURCE_DIR}/shadow.png
		${ASSETS_SOURCE_DIR}/playcircle.svg
		${ASSETS_SOURCE_DIR}/stopcircle.svg
		${ASSETS_SOURCE_DIR}/shift.svg
		${ASSETS_SOURCE_DIR}/button.svg
)

# Option to enable plugin hosting
option(ENABLE_PLUGIN_HOSTING "Enable plugin hosting in the app" ON)

# Shared Code Library
add_library(trackDeck_SharedCode STATIC)
file(GLOB SHARED_SOURCE_FILES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Components/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.h"
)
target_sources(trackDeck_SharedCode PRIVATE ${SHARED_SOURCE_FILES})
target_link_libraries(trackDeck_SharedCode PUBLIC ${JUCE_MODULES} ${TRACKTION_MODULES} Assets)
target_include_directories(trackDeck_SharedCode PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Source
		${CMAKE_CURRENT_BINARY_DIR}/Assets/JuceLibraryCode
)
target_compile_definitions(trackDeck_SharedCode PUBLIC
		JUCE_WEB_BROWSER=0
		JUCE_USE_CURL=0
		JUCE_VST3_CAN_REPLACE_VST2=0
		JUCE_MODAL_LOOPS_PERMITTED=1
		CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
		VERSION="${PROJECT_VERSION}"
		BUILD_COMMIT_HASH="${GIT_COMMIT_HASH}"
		BUILD_TAG="${GIT_TAG}"
		PRODUCT_NAME_WITHOUT_VERSION="trackDeck"
		$<$<CONFIG:Debug>:USE_MELATONIN_INSPECTOR>
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_libraries(trackDeck_SharedCode PUBLIC melatonin_inspector)
endif()

# Application: trackDeck
juce_add_gui_app(
		trackDeck
		PRODUCT_NAME "trackDeck"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.apps.trackDeck"
		TARGETED_DEVICE_FAMILY "2"
		IPHONE_SCREEN_ORIENTATION "Landscape"
		MICROPHONE_PERMISSION_ENABLED TRUE
		DOCUMENT_BROWSER_ENABLED FALSE
		FILE_SHARING_ENABLED FALSE
		REQUIRES_FULL_SCREEN TRUE
		IPAD_SCREEN_ORIENTATIONS "LandscapeLeft;LandscapeRight"
)

file(GLOB APP_SOURCE_FILES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.h"
)
target_sources(trackDeck PRIVATE ${APP_SOURCE_FILES})
target_link_libraries(trackDeck PRIVATE trackDeck_SharedCode ${JUCE_MODULES} ${TRACKTION_MODULES} Assets)
target_compile_definitions(trackDeck PRIVATE
		JUCE_IOS=1
		JUCE_STANDALONE_APPLICATION=1
		$<$<BOOL:${ENABLE_PLUGIN_HOSTING}>:JUCE_PLUGINHOST_AU=1>
		$<$<BOOL:${ENABLE_PLUGIN_HOSTING}>:JUCE_PLUGINHOST_VST3=1>
)

if(IOS)
	set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")
	set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS architectures")
	set_target_properties(trackDeck PROPERTIES
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
			XCODE_ATTRIBUTE_UIFileSharingEnabled "NO"
			XCODE_ATTRIBUTE_UIBackgroundModes "audio"
	)
elseif(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build for Apple Silicon and Intel")
endif()

# Plugin: trackDeck
juce_add_plugin(
		trackDeckPlugin
		PRODUCT_NAME "trackDeck"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.plugins.trackDeck"
		IS_SYNTH TRUE
		NEEDS_MIDI_INPUT TRUE
		NEEDS_MIDI_OUTPUT TRUE
		IS_MIDI_EFFECT TRUE
		EDITOR_WANTS_KEYBOARD_FOCUS TRUE
		COPY_PLUGIN_AFTER_BUILD ON
		PLUGIN_MANUFACTURER_CODE ENIO
		PLUGIN_CODE STEP
		FORMATS Standalone AU VST3 AUv3
)

target_sources(trackDeckPlugin PRIVATE ${SHARED_SOURCE_FILES})
target_link_libraries(trackDeckPlugin PRIVATE trackDeck_SharedCode ${JUCE_MODULES} ${TRACKTION_MODULES} Assets)
target_compile_definitions(trackDeckPlugin PRIVATE
		JUCE_IOS=1
)

if(IOS)
	set_target_properties(trackDeckPlugin PROPERTIES
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
	)
endif()

# C++17 workaround
foreach(target trackDeck trackDeckPlugin trackDeck_SharedCode)
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
		target_compile_options(${target} PRIVATE -std=c++17)
	endif()
endforeach()