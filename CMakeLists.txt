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
set(ASSETS_DEST_DEBUG "${CMAKE_BINARY_DIR}/trackDeck_artefacts/Debug/Standalone/Assets")
set(ASSETS_DEST_RELEASE "${CMAKE_BINARY_DIR}/trackDeck_artefacts/Release/Standalone/Assets")

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
# APPLICATION: trackDeck (iOS GUI App)
# ==============================================================================
project(trackDeckApp VERSION 1.0)

juce_add_gui_app(
		trackDeckAPP
		PRODUCT_NAME "trackDeck"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.apps.trackDeck"
		TARGETED_DEVICE_FAMILY "2" # 1 = iPhone, 2 = iPad, "1,2" = Universal
		IPHONE_SCREEN_ORIENTATION "Landscape" # or "Landscape", "PortraitAndLandscape"
		MICROPHONE_PERMISSION_ENABLED TRUE
		DOCUMENT_BROWSER_ENABLED FALSE # Optional iOS-specific setting
		FILE_SHARING_ENABLED FALSE     # Optional iOS-specific setting
		REQUIRES_FULL_SCREEN TRUE      # Optional: Full-screen app
		IPAD_SCREEN_ORIENTATIONS "LandscapeLeft;LandscapeRight" # Specific to iPad
)

# Automatically add all application source files
file(GLOB_RECURSE APP_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/App/*.h)

target_sources(trackDeckAPP PRIVATE ${APP_SOURCE_FILES})

# Link JUCE modules
target_link_libraries(trackDeckAPP PRIVATE ${JUCE_MODULES})

# iOS-specific settings
if(IOS)
	# Set iOS deployment target
	set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")

	# Set architectures for iOS (arm64 for devices, arm64/x86_64 for simulator)
	set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS architectures")

	# Add iOS-specific compile definitions
	target_compile_definitions(trackDeckAPP PRIVATE
			JUCE_IOS=1
			JUCE_STANDALONE_APPLICATION=1
	)

	# Ensure the app is signed (required for iOS builds)
	set_target_properties(trackDeckAPP PROPERTIES
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "PBB4749V9S" # Replace with your Apple Developer Team ID
			XCODE_ATTRIBUTE_UIFileSharingEnabled "NO"
			XCODE_ATTRIBUTE_UIBackgroundModes "audio"
	)
endif()

# Add this after the existing trackDeckAPP target definition to create an Xcode-specific iOS target for iPad
juce_add_gui_app(
		trackDeckAPP_iOS_Xcode
		PRODUCT_NAME "trackDeck"
		COMPANY_NAME "enabled.io"
		BUNDLE_ID "io.enabled.apps.trackDeck"
		TARGETED_DEVICE_FAMILY "2" # iPad only
		IPHONE_SCREEN_ORIENTATION "Landscape"
		MICROPHONE_PERMISSION_ENABLED TRUE
		DOCUMENT_BROWSER_ENABLED FALSE
		FILE_SHARING_ENABLED FALSE
		REQUIRES_FULL_SCREEN TRUE
		IPAD_SCREEN_ORIENTATIONS "LandscapeLeft;LandscapeRight"
)

target_sources(trackDeckAPP_iOS_Xcode PRIVATE ${APP_SOURCE_FILES})
target_link_libraries(trackDeckAPP_iOS_Xcode PRIVATE ${JUCE_MODULES})

# iOS Xcode-specific settings
set_target_properties(trackDeckAPP_iOS_Xcode PROPERTIES
		XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0"
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
		XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID" # Replace with your Apple Developer Team ID
		CMAKE_OSX_ARCHITECTURES "arm64"
		GENERATOR "Xcode" # Forces Xcode generator instead of Ninja
		XCODE_ATTRIBUTE_UIFileSharingEnabled "NO"
		XCODE_ATTRIBUTE_UIBackgroundModes "audio"
)

target_compile_definitions(trackDeckAPP_iOS_Xcode PRIVATE
		JUCE_IOS=1
		JUCE_STANDALONE_APPLICATION=1
)

# ==============================================================================
# PLUGIN: trackDeck
# ==============================================================================
project(trackDeck VERSION 1.0.0)

set(FORMATS Standalone AU VST3 AUv3) # AUv3 is iOS-compatible

juce_add_plugin(
		trackDeck
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
		FORMATS ${FORMATS}
)

# Automatically add all plugin source files
file(GLOB_RECURSE PLUGIN_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/Components/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.h)

target_sources(trackDeck PRIVATE ${PLUGIN_SOURCE_FILES})

# Set plugin compile definitions
target_compile_definitions(trackDeck
		INTERFACE
		JUCE_WEB_BROWSER=0
		JUCE_USE_CURL=0
		JUCE_VST3_CAN_REPLACE_VST2=0
		JUCE_MODAL_LOOPS_PERMITTED=1
		CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
		VERSION="${PROJECT_VERSION}"
		BUILD_COMMIT_HASH="${GIT_COMMIT_HASH}"
		BUILD_TAG="${GIT_TAG}"
		PRODUCT_NAME_WITHOUT_VERSION="trackDeck"
)

juce_add_binary_data(Assets
		SOURCES
		Assets/back.svg
		Assets/Conthrax-SemiBold.otf
		Assets/cuttlefish.jpg
		Assets/shadow.png
		Assets/playcircle.svg
		Assets/stopcircle.svg
		Assets/shift.svg
		Assets/button.svg
)

# Link JUCE modules
target_link_libraries(trackDeck PRIVATE ${JUCE_MODULES} Assets)

# iOS-specific settings for the plugin (if targeting iOS)
if(IOS)
	target_compile_definitions(trackDeck PRIVATE
			JUCE_IOS=1
	)
	set_target_properties(trackDeck PROPERTIES
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXX" # !J! TODO: Work with Tom to get enabled.io Team set up
	)
endif()

# Set macOS architectures for universal binary (Apple Silicon + Intel)
if(APPLE AND NOT IOS)
	message("Building on macOS (Darwin)")
	target_link_options(trackDeck INTERFACE -Wl)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build for Apple Silicon and Intel")
endif()