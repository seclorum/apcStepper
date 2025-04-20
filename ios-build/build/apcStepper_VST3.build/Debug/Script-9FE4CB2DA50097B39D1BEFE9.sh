#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_VST3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3/Contents/moduleinfo.json /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo "creating /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3/Contents/Resources
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/Debug/juce_vst3_helper -create -version 1.0.0 -path /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3 -output /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/VST3/apcStepper.vst3 -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/VST3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_VST3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3/Contents/moduleinfo.json /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo "creating /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3/Contents/Resources
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/Release/juce_vst3_helper -create -version 1.0.0 -path /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3 -output /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/VST3/apcStepper.vst3 -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/VST3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_VST3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3/Contents/moduleinfo.json /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo "creating /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3/Contents/Resources
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/MinSizeRel/juce_vst3_helper -create -version 1.0.0 -path /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3 -output /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/VST3/apcStepper.vst3 -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/VST3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_VST3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo removing\ moduleinfo.json
  /opt/homebrew/bin/cmake -E remove -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3/Contents/moduleinfo.json /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E echo "creating /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json"
  /opt/homebrew/bin/cmake -E make_directory /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3/Contents/Resources
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/RelWithDebInfo/juce_vst3_helper -create -version 1.0.0 -path /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3 -output /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3/Contents/Resources/moduleinfo.json
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/VST3/apcStepper.vst3 -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/VST3 -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi

