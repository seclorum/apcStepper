# Install script for directory: /Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Library/Developer/CommandLineTools/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-debug/_deps/tracktion_engine-build/modules/juce/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-debug/_deps/tracktion_engine-build/modules/juce/extras/Build/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-8.0.4" TYPE FILE FILES
    "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-debug/_deps/tracktion_engine-build/modules/juce/JUCEConfigVersion.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-debug/_deps/tracktion_engine-build/modules/juce/JUCEConfig.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/JUCECheckAtomic.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/JUCEHelperTargets.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/JUCEModuleSupport.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/JUCEUtils.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/JuceLV2Defines.h.in"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/LaunchScreen.storyboard"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/PIPComponent.cpp.in"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/PIPConsole.cpp.in"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/checkBundleSigning.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/copyDir.cmake"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    "/Users/tompeakwalcher/Documents/develop/apcStepper/modules/tracktion_engine/modules/juce/extras/Build/CMake/juce_LinuxSubprocessHelper.cpp"
    )
endif()

