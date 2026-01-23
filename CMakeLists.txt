cmake_minimum_required(VERSION 3.14)
project(dali-react-native CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(DALI REQUIRED IMPORTED_TARGET dali2-core dali2-adaptor dali2-toolkit)

# Include React Native and Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(ReactNative REQUIRED)

include_directories(
    ${React_INCLUDE_DIRS}
    ${React_ThirdParty_INCLUDE_DIRS}
    ${HERMES_INCLUDE_DIRS}
)

add_definitions(-DFOLLY_NO_CONFIG -DFOLLY_MOBILE=1 -DFOLLY_USE_LIBCPP=1 -DRN_FABRIC_ENABLED)

add_executable(dali-rn-demo
  src/main.cpp
  src/DaliRenderer.cpp
  src/DeviceInstanceManager.cpp
  src/DaliMountingManager.cpp
  src/components/DaliViewComponent.cpp
  src/components/DaliTextComponent.cpp
  src/components/DaliImageComponent.cpp
)

target_link_libraries(dali-rn-demo
  PRIVATE
  PkgConfig::DALI
  ${React_LIBRARIES}
  "-framework CoreFoundation" # Required by Folly/RN on macOS
  "-framework JavaScriptCore" # For JSCRuntime
)

target_include_directories(dali-rn-demo PRIVATE
  src
  ${DALI_INCLUDE_DIRS}

  # ${React_INCLUDE_DIRS}
  # ${React_ThirdParty_INCLUDE_DIRS}
  # ${Hermes_INCLUDE_DIRS}
)
