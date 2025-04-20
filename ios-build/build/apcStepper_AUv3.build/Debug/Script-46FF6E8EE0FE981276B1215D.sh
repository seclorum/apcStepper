#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/modules/juce/tools/extras/Build/juceaide/juceaide_artefacts/Debug/juceaide pkginfo "AUv3 AppExtension" /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/modules/juce/tools/extras/Build/juceaide/juceaide_artefacts/Debug/juceaide pkginfo "AUv3 AppExtension" /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/modules/juce/tools/extras/Build/juceaide/juceaide_artefacts/Debug/juceaide pkginfo "AUv3 AppExtension" /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/modules/juce/tools/extras/Build/juceaide/juceaide_artefacts/Debug/juceaide pkginfo "AUv3 AppExtension" /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/apcStepper_artefacts/JuceLibraryCode/apcStepper_AUv3/PkgInfo
fi

