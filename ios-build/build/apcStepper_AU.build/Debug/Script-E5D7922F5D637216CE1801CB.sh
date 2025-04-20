#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AU/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/AU/apcStepper.component/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/AU/apcStepper.component -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Debug/AU/apcStepper.component -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/Components -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AU/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/AU/apcStepper.component/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/AU/apcStepper.component -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/Release/AU/apcStepper.component -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/Components -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AU/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/AU/apcStepper.component/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/AU/apcStepper.component -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/MinSizeRel/AU/apcStepper.component -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/Components -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -E copy /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AU/PkgInfo /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/AU/apcStepper.component/Contents
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/AU/apcStepper.component -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/checkBundleSigning.cmake
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /opt/homebrew/bin/cmake -Dsrc=/Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/RelWithDebInfo/AU/apcStepper.component -Ddest=/Users/jayvaughan/Library/Audio/Plug-Ins/Components -P /Users/jayvaughan/Documents/Development/Plugins/apcStepper/modules/juce/extras/Build/CMake/copyDir.cmake
fi

