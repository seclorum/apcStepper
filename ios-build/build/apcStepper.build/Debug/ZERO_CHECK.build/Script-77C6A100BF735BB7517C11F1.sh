#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  make -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  make -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  make -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build
  make -f /Users/jayvaughan/Documents/Development/Plugins/apcStepper/ios-build/CMakeScripts/ReRunCMake.make
fi

