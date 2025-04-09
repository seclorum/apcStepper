cmake_minimum_required(VERSION 3.15)

# Enable verbose output for build process
set(CMAKE_VERBOSE_MAKEFILE ON)

# JUCE Setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(JUCE_ENABLE_LV2 FALSE)
set(JUCE_ENABLE_UNITY FALSE)

# Add JUCE modules
message(STATUS "Adding JUCE submodule from modules/juce")
add_subdirectory(modules/juce)
message(STATUS "Adding Melatonin Inspector submodule from modules/melatonin_inspector")
add_subdirectory(modules/melatonin_inspector)
message(STATUS "Adding Tracktion Engine submodule from modules/tracktion_engine")
add_subdirectory(modules/tracktion_engine) # Corrected path

set(ASSETS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Source/Assets")
set(ASSETS_DEST_DEBUG "${CMAKE_BINARY_DIR}/apcStepper_artefacts/Debug/Standalone/Assets")
set(ASSETS_DEST_RELEASE "${CMAKE_BINARY_DIR}/apcStepper_artefacts/Release/Standalone/Assets")

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
message(STATUS "Git Commit Hash: ${GIT_COMMIT_HASH}")

# Get the latest Git tag (avoid errors if not in Git repo)
execute_process(
		COMMAND git describe --tags --abbrev=0
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_TAG
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
)
message(STATUS "Git Tag: ${GIT_TAG}")

# Check the hostname and set AAX_SDK_PATH if needed
execute_process(
		COMMAND uname -n
		OUTPUT_VARIABLE HOSTNAME
		OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Hostname: ${HOSTNAME}")

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
		tracktion_engine
)
message(STATUS "JUCE Modules configured: ${JUCE_MODULES}")

# ==============================================================================
# APPLICATION: apcStepper (iOS GUI App)
# ==============================================================================
project(apcAPP VERSION 1.0)
message(STATUS "Configuring project: apcAPP")

juce_add_gui_app(
		apcAPP
		PRODUCT_NAME "apcStepper"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.apc.Stepper"
		TARGETED_DEVICE_FAMILY "2" # 1 = iPhone, 2 = iPad, "1,2" = Universal
		IPHONE_SCREEN_ORIENTATION "Landscape"
		MICROPHONE_PERMISSION_ENABLED TRUE
		DOCUMENT_BROWSER_ENABLED FALSE
		FILE_SHARING_ENABLED FALSE
		REQUIRES_FULL_SCREEN TRUE
		IPAD_SCREEN_ORIENTATIONS "LandscapeLeft;LandscapeRight"
)
message(STATUS "Added GUI app: apcAPP")

# Automatically add all application source files
file(GLOB_RECURSE APP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.h)
message(STATUS "Application source files: ${APP_SOURCE_FILES}")

target_sources(apcAPP PRIVATE ${APP_SOURCE_FILES})

# Link JUCE modules
target_link_libraries(apcAPP PRIVATE ${JUCE_MODULES})
message(STATUS "Linked modules to apcAPP: ${JUCE_MODULES}")

# iOS-specific settings
if(IOS)
	message(STATUS "Applying iOS-specific settings for apcAPP")
	set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")
	set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS architectures")

	target_compile_definitions(apcAPP PRIVATE
			JUCE_IOS=1
			JUCE_STANDALONE_APPLICATION=1
	)

	set_target_properties(apcAPP PROPERTIES
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID" # Replace with your Apple Developer Team ID
	)

	juce_set_custom_xcode_plist_properties(apcAPP
			"UIFileSharingEnabled" "NO"
			"UIBackgroundModes" "audio"
	)
endif()

# ==============================================================================
# PLUGIN: apcStepper
# ==============================================================================
project(apcStepper VERSION 1.0.0)
message(STATUS "Configuring project: apcStepper")

set(FORMATS Standalone AU VST3 AUv3)
message(STATUS "Plugin formats: ${FORMATS}")

juce_add_plugin(
		apcStepper
		PRODUCT_NAME "apcStepper"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.apc.Stepper"
		IS_SYNTH TRUE
		NEEDS_MIDI_INPUT TRUE
		NEEDS_MIDI_OUTPUT TRUE
		IS_MIDI_EFFECT TRUE
		EDITOR_WANTS_KEYBOARD_FOCUS TRUE
		COPY_PLUGIN_AFTER_BUILD ON
		PLUGIN_MANUFACTURER_CODE ENIO
		PLUGIN_CODE STEP
		FORMATS ${FORMATS}
)
message(STATUS "Added plugin: apcStepper")

# Automatically add all plugin source files
file(GLOB_RECURSE PLUGIN_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Source/Components/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.h
)
# Explicitly add apcTrackView.cpp to avoid missing file errors
list(APPEND PLUGIN_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/apcTrackView.cpp)
message(STATUS "Plugin source files: ${PLUGIN_SOURCE_FILES}")

target_sources(apcStepper PRIVATE ${PLUGIN_SOURCE_FILES})

# Set plugin compile definitions
target_compile_definitions(apcStepper
		INTERFACE
		JUCE_WEB_BROWSER=0
		JUCE_USE_CURL=0
		JUCE_VST3_CAN_REPLACE_VST2=0
		JUCE_MODAL_LOOPS_PERMITTED=1
		CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
		VERSION="${PROJECT_VERSION}"
		BUILD_COMMIT_HASH="${GIT_COMMIT_HASH}"
		BUILD_TAG="${GIT_TAG}"
		PRODUCT_NAME_WITHOUT_VERSION="apcStepper"
)

# Add binary data (Assets)
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
message(STATUS "Added binary data: Assets")

# Link JUCE modules
target_link_libraries(apcStepper PRIVATE ${JUCE_MODULES} Assets)
message(STATUS "Linked modules to apcStepper: ${JUCE_MODULES}, Assets")

# iOS-specific settings for the plugin
if(IOS)
	message(STATUS "Applying iOS-specific settings for apcStepper")
	target_compile_definitions(apcStepper PRIVATE
			JUCE_IOS=1
	)
	set_target_properties(apcStepper PROPERTIES
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXX" # TODO: Replace with enabled.io Team ID
	)
endif()

# Set macOS architectures for universal binary
if(APPLE AND NOT IOS)
	message(STATUS "Building on macOS (Darwin)")
	target_link_options(apcStepper INTERFACE -Wl)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build for Apple Silicon and Intel")
endif()