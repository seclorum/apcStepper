# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.15)

# Project setup (Include C to fix CMAKE_C_COMPILE_OBJECT issue)
project(trackDeck VERSION 1.0.0 LANGUAGES CXX C)

# JUCE and Tracktion Engine setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

# Set SDKs for iOS and macOS
if(IOS)
	execute_process(
			COMMAND xcrun --sdk iphoneos --show-sdk-path
			OUTPUT_VARIABLE IPHONEOS_SDK_PATH
			OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE SDK_CHECK_RESULT
	)
	if(NOT SDK_CHECK_RESULT EQUAL 0 OR NOT EXISTS "${IPHONEOS_SDK_PATH}")
		message(FATAL_ERROR "iPhoneOS SDK not found at ${IPHONEOS_SDK_PATH}. Ensure Xcode and Command Line Tools are installed.")
	endif()
	set(CMAKE_OSX_SYSROOT "${IPHONEOS_SDK_PATH}" CACHE STRING "iOS SDK" FORCE)
	message(STATUS "Using iPhoneOS SDK: ${CMAKE_OSX_SYSROOT}")
elseif(APPLE)
	execute_process(
			COMMAND xcrun --sdk macosx --show-sdk-path
			OUTPUT_VARIABLE MACOSX_SDK_PATH
			OUTPUT_STRIP_TRAILING_WHITESPACE
			RESULT_VARIABLE SDK_CHECK_RESULT
	)
	if(NOT SDK_CHECK_RESULT EQUAL 0 OR NOT EXISTS "${MACOSX_SDK_PATH}")
		message(FATAL_ERROR "macOS SDK not found at ${MACOSX_SDK_PATH}. Ensure Xcode and Command Line Tools are installed.")
	endif()
	set(CMAKE_OSX_SYSROOT "${MACOSX_SDK_PATH}" CACHE STRING "macOS SDK" FORCE)
	message(STATUS "Using macOS SDK: ${CMAKE_OSX_SYSROOT}")
endif()

# Custom function for Git fetching with real progress bar and submodule support
function(fetch_with_progress repo_name url tag source_dir)
	message(STATUS "Fetching ${repo_name}...")
	include(ExternalProject)
	ExternalProject_Add(
			${repo_name}_fetch
			GIT_REPOSITORY ${url}
			GIT_TAG ${tag}
			SOURCE_DIR ${source_dir}
			CONFIGURE_COMMAND ""
			BUILD_COMMAND ""
			INSTALL_COMMAND ""
			TEST_COMMAND ""
			GIT_PROGRESS TRUE
			GIT_CONFIG advice.detachedHead=false
			LOG_DOWNLOAD ON
	)
	set(progress_file "${CMAKE_BINARY_DIR}/${repo_name}_progress.txt")
	file(REMOVE ${progress_file})
	execute_process(COMMAND ${CMAKE_COMMAND} -E touch ${progress_file})
	set(max_stars 20)
	set(current_percent 0)
	while(${current_percent} LESS 100)
		if(EXISTS "${source_dir}/.git")
			set(current_percent 100)
		else()
			execute_process(
					COMMAND git clone ${url} ${source_dir} --progress
					OUTPUT_VARIABLE git_output
					ERROR_VARIABLE git_error
					RESULT_VARIABLE git_result
					OUTPUT_QUIET
			)
			string(REGEX MATCH "Receiving objects:[ \t]*([0-9]+)%" match "${git_error}")
			if(match)
				set(current_percent ${CMAKE_MATCH_1})
			else()
				set(current_percent 0)
			endif()
		endif()
		math(EXPR stars "(${current_percent} * ${max_stars}) / 100")
		string(REPEAT "*" ${stars} star_bar)
		message(STATUS "[${star_bar}${current_percent}%] Cloning ${repo_name}")
		execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1)
	endwhile()
	if(EXISTS "${source_dir}/.gitmodules")
		message(STATUS "Initializing submodules for ${repo_name}...")
		set(submodule_percent 0)
		execute_process(
				COMMAND git submodule update --init --recursive
				WORKING_DIRECTORY ${source_dir}
				OUTPUT_VARIABLE submodule_output
				ERROR_VARIABLE submodule_error
				RESULT_VARIABLE submodule_result
		)
		while(${submodule_percent} LESS 100)
			if(${submodule_result} EQUAL 0)
				set(submodule_percent 100)
			else()
				set(submodule_percent 50)
			endif()
			math(EXPR sub_stars "(${submodule_percent} * ${max_stars}) / 100")
			string(REPEAT "*" ${sub_stars} sub_star_bar)
			message(STATUS "[${sub_star_bar}${submodule_percent}%] Initializing submodules for ${repo_name}")
			execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1)
			if(EXISTS "${source_dir}/modules/juce")
				set(submodule_percent 100)
			endif()
		endwhile()
		if(NOT ${submodule_result} EQUAL 0)
			message(FATAL_ERROR "Failed to initialize submodules for ${repo_name}: ${submodule_error}")
		endif()
		message(STATUS "Submodules for ${repo_name} initialized successfully!")
	endif()
	FetchContent_Declare(${repo_name} SOURCE_DIR ${source_dir})
	FetchContent_MakeAvailable(${repo_name})
	message(STATUS "${repo_name} fetched successfully!")
endfunction()

# Cache FetchContent downloads
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps_cache" CACHE PATH "Directory for cached FetchContent downloads")

# Fetch Tracktion Engine
include(FetchContent)
fetch_with_progress(
		tracktion_engine
		https://github.com/Tracktion/tracktion_engine.git
		origin/develop
		${CMAKE_CURRENT_BINARY_DIR}/tracktion_engine
)
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/tracktion_engine/modules/juce")
	message(FATAL_ERROR "JUCE submodule not found at ${CMAKE_CURRENT_BINARY_DIR}/tracktion_engine/modules/juce.")
endif()
FetchContent_GetProperties(tracktion_engine)
if(NOT tracktion_engine_POPULATED)
	FetchContent_Populate(tracktion_engine)
	add_subdirectory(${tracktion_engine_SOURCE_DIR}/modules ${tracktion_engine_BINARY_DIR})
endif()

# Include Melatonin Inspector for Debug builds
if(CMAKE_BUILD_TYPE MATCHES Debug)
	fetch_with_progress(
			melatonin_inspector
			https://github.com/sudara/melatonin_inspector.git
			origin/main
			${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector
	)
endif()

# Define asset directories
set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Assets")
set(ASSETS_DEST_DEBUG "${CMAKE_BINARY_DIR}/trackDeck_artefacts/Debug/Standalone/Assets")
set(ASSETS_DEST_RELEASE "${CMAKE_BINARY_DIR}/trackDeck_artefacts/Release/Standalone/Assets")

# Copy assets
file(GLOB ASSET_FILES "${ASSETS_SOURCE_DIR}/*")
foreach(asset ${ASSET_FILES})
	get_filename_component(asset_name ${asset} NAME)
	configure_file(${asset} ${ASSETS_DEST_DEBUG}/${asset_name} COPYONLY)
	configure_file(${asset} ${ASSETS_DEST_RELEASE}/${asset_name} COPYONLY)
endforeach()

# Get Git commit hash
if(NOT GIT_COMMIT_HASH)
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
	set(GIT_COMMIT_HASH ${GIT_COMMIT_HASH} CACHE STRING "Git commit hash")
endif()

# Get latest Git tag
if(NOT GIT_TAG)
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
	set(GIT_TAG ${GIT_TAG} CACHE STRING "Git tag")
endif()

# Define JUCE modules
set(JUCE_MODULES
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
		juce_audio_plugin_client
		juce_recommended_config_flags
		juce_recommended_lto_flags
		juce_recommended_warning_flags
)

# Define Tracktion Engine modules
set(TRACKTION_MODULES
		tracktion_engine
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

# Common compile definitions
set(COMMON_COMPILE_DEFS
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

# Shared Code Library - iOS
add_library(trackDeck_SharedCode_iOS STATIC)
file(GLOB SHARED_SOURCE_FILES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Components/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.h"
)
target_sources(trackDeck_SharedCode_iOS PRIVATE ${SHARED_SOURCE_FILES})
target_link_libraries(trackDeck_SharedCode_iOS PUBLIC
		${JUCE_MODULES}
		${TRACKTION_MODULES}
		Assets
		"-framework UIKit"
		"-framework CoreAudio"
		"-framework AVFoundation"
		"-framework CoreMIDI"
)
target_include_directories(trackDeck_SharedCode_iOS PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Source
		${CMAKE_CURRENT_BINARY_DIR}/Assets/JuceLibraryCode
)
target_compile_definitions(trackDeck_SharedCode_iOS PUBLIC
		${COMMON_COMPILE_DEFS}
		JUCE_IOS=1
)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_libraries(trackDeck_SharedCode_iOS PUBLIC melatonin_inspector)
endif()

# Shared Code Library - macOS
add_library(trackDeck_SharedCode_Mac STATIC)
target_sources(trackDeck_SharedCode_Mac PRIVATE ${SHARED_SOURCE_FILES})
target_link_libraries(trackDeck_SharedCode_Mac PUBLIC
		${JUCE_MODULES}
		${TRACKTION_MODULES}
		Assets
		"-framework Cocoa"
		"-framework CoreAudio"
		"-framework AVFoundation"
		"-framework CoreMIDI"
)
target_include_directories(trackDeck_SharedCode_Mac PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Source
		${CMAKE_CURRENT_BINARY_DIR}/Assets/JuceLibraryCode
)
target_compile_definitions(trackDeck_SharedCode_Mac PUBLIC
		${COMMON_COMPILE_DEFS}
		JUCE_MAC=1
)
if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_libraries(trackDeck_SharedCode_Mac PUBLIC melatonin_inspector)
endif()

# Application: trackDeck - iOS
juce_add_gui_app(
		trackDeck_iOS
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
target_sources(trackDeck_iOS PRIVATE ${APP_SOURCE_FILES})
target_link_libraries(trackDeck_iOS PRIVATE
		trackDeck_SharedCode_iOS
		${JUCE_MODULES}
		${TRACKTION_MODULES}
		Assets
		"-framework UIKit"
		"-framework CoreAudio"
		"-framework AVFoundation"
		"-framework CoreMIDI"
)
target_compile_definitions(trackDeck_iOS PRIVATE
		JUCE_IOS=1
		JUCE_STANDALONE_APPLICATION=1
		$<$<BOOL:${ENABLE_PLUGIN_HOSTING}>:JUCE_PLUGINHOST_AU=1>
		$<$<BOOL:${ENABLE_PLUGIN_HOSTING}>:JUCE_PLUGINHOST_VST3=1>
)
set_target_properties(trackDeck_iOS PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
		XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
		XCODE_ATTRIBUTE_UIFileSharingEnabled "NO"
		XCODE_ATTRIBUTE_UIBackgroundModes "audio"
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0"
		XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "2"
)

# Application: trackDeck - macOS
juce_add_gui_app(
		trackDeck_Mac
		PRODUCT_NAME "trackDeck"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.apps.trackDeck"
)
target_sources(trackDeck_Mac PRIVATE ${APP_SOURCE_FILES})
target_link_libraries(trackDeck_Mac PRIVATE
		trackDeck_SharedCode_Mac
		${JUCE_MODULES}
		${TRACKTION_MODULES}
		Assets
		"-framework Cocoa"
		"-framework CoreAudio"
		"-framework AVFoundation"
		"-framework CoreMIDI"
)
target_compile_definitions(trackDeck_Mac PRIVATE
		JUCE_MAC=1
		JUCE_STANDALONE_APPLICATION=1
		$<$<BOOL:${ENABLE_PLUGIN_HOSTING}>:JUCE_PLUGINHOST_AU=1>
		$<$<BOOL:${ENABLE_PLUGIN_HOSTING}>:JUCE_PLUGINHOST_VST3=1>
)
set_target_properties(trackDeck_Mac PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Mac Developer"
		XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
		XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.13"
)

# Plugin: trackDeck - iOS
juce_add_plugin(
		trackDeckPlugin_iOS
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
target_sources(trackDeckPlugin_iOS PRIVATE ${SHARED_SOURCE_FILES})
target_link_libraries(trackDeckPlugin_iOS PRIVATE
		trackDeck_SharedCode_iOS
		${JUCE_MODULES}
		${TRACKTION_MODULES}
		Assets
		"-framework UIKit"
		"-framework CoreAudio"
		"-framework AVFoundation"
		"-framework CoreMIDI"
)
target_compile_definitions(trackDeckPlugin_iOS PRIVATE
		JUCE_IOS=1
)
set_target_properties(trackDeckPlugin_iOS PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
		XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0"
)

# Plugin: trackDeck - macOS
juce_add_plugin(
		trackDeckPlugin_Mac
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
		FORMATS Standalone AU VST3
)
target_sources(trackDeckPlugin_Mac PRIVATE ${SHARED_SOURCE_FILES})
target_link_libraries(trackDeckPlugin_Mac PRIVATE
		trackDeck_SharedCode_Mac
		${JUCE_MODULES}
		${TRACKTION_MODULES}
		Assets
		"-framework Cocoa"
		"-framework CoreAudio"
		"-framework AVFoundation"
		"-framework CoreMIDI"
)
target_compile_definitions(trackDeckPlugin_Mac PRIVATE
		JUCE_MAC=1
)
set_target_properties(trackDeckPlugin_Mac PROPERTIES
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Mac Developer"
		XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
		XCODE_ATTRIBUTE_MACOSX_DEPLOYMENT_TARGET "10.13"
)

# Platform-specific architectures
if(IOS)
	set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS architectures")
elseif(APPLE)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures")
endif()

# Apply compile options based on file type
foreach(target trackDeck_SharedCode_iOS trackDeck_iOS trackDeck_Mac trackDeckPlugin_iOS trackDeckPlugin_Mac)
	if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
		get_target_property(SOURCE_FILES ${target} SOURCES)
		foreach(source ${SOURCE_FILES})
			if(source MATCHES "\\.cpp$" OR source MATCHES "\\.mm$")
				set_source_files_properties(${source} PROPERTIES COMPILE_OPTIONS -std=c++20) # Updated to C++20
			endif()
			if(source MATCHES "\\.c$")
				set_source_files_properties(${source} PROPERTIES COMPILE_OPTIONS -std=c11)
			endif()
		endforeach()
	endif()
endforeach()