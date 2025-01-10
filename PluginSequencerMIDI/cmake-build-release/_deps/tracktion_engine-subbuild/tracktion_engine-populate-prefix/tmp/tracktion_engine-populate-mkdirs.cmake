# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/../modules/tracktion_engine")
  file(MAKE_DIRECTORY "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/../modules/tracktion_engine")
endif()
file(MAKE_DIRECTORY
  "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-build"
  "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix"
  "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix/tmp"
  "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix/src/tracktion_engine-populate-stamp"
  "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix/src"
  "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix/src/tracktion_engine-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix/src/tracktion_engine-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/tompeakwalcher/Documents/develop/apcStepper/PluginSequencerMIDI/cmake-build-release/_deps/tracktion_engine-subbuild/tracktion_engine-populate-prefix/src/tracktion_engine-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
