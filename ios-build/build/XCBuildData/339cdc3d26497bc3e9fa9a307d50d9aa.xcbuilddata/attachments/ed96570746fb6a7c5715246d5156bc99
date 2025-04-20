#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/AUv3/apcStepper.appex/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/AUv3/apcStepper.appex -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/AUv3/apcStepper.appex/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/AUv3/apcStepper.appex -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/AUv3/apcStepper.appex/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/AUv3/apcStepper.appex -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/AUv3/apcStepper.appex/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/AUv3/apcStepper.appex -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
fi

