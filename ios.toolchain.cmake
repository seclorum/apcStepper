set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_SYSROOT iphoneos)
set(CMAKE_OSX_ARCHITECTURES arm64) # Target arm64 for modern iOS devices
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO") # Disable for simulator or adjust for device
set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO") # Optional
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0") # Minimum iOS version
