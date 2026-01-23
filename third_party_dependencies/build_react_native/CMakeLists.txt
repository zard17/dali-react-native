cmake_minimum_required(VERSION 3.14)
project(ReactNativeBuild CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Definitions expected by RN
add_definitions(-DFOLLY_NO_CONFIG -DFOLLY_MOBILE=1 -DFOLLY_USE_LIBCPP=1 -DRN_FABRIC_ENABLED)

# Mock Macros required by ReactCommon
macro(react_native_android_selector OUT_VAR ANDROID_VAL GENERIC_VAL)
    if(ANDROID)
        set(${OUT_VAR} "${ANDROID_VAL}")
    else()
        set(${OUT_VAR} "${GENERIC_VAL}")
    endif()
endmacro()

macro(target_merge_so target)
    # No-op for static build
endmacro()

# Set REACT_COMMON_DIR
get_filename_component(NODE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../node_modules" ABSOLUTE)
set(REACT_COMMON_DIR "${NODE_MODULES_DIR}/react-native/ReactCommon")
set(REACT_ANDROID_DIR "${NODE_MODULES_DIR}/react-native/ReactAndroid")

# Find Dependencies
set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../install")

find_package(folly CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(double-conversion CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# Aliases for RN
add_library(folly_runtime ALIAS Folly::folly)
add_library(glog ALIAS glog::glog)
# Alias glog_init (used by react_utils) to glog
add_library(glog_init ALIAS glog::glog)
add_library(double-conversion ALIAS double-conversion::double-conversion)

# Yoga
add_library(yoga STATIC IMPORTED)
set_target_properties(yoga PROPERTIES
    IMPORTED_LOCATION "${CMAKE_PREFIX_PATH}/lib/libyogacore.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_PREFIX_PATH}/include"
)
add_library(yogacore ALIAS yoga)

# Boost
set(BOOST_ROOT "${NODE_MODULES_DIR}/../../../third_party_dependencies/boost_1_83_0")
add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${BOOST_ROOT})

# Helper to look into subdirectories
macro(add_rn_subdir path)
    add_subdirectory("${REACT_COMMON_DIR}/${path}" "${CMAKE_CURRENT_BINARY_DIR}/${path}")
endmacro()

# Core ReactCommon
add_rn_subdir(cxxreact)
add_rn_subdir(jsi)
add_rn_subdir(logger)
add_rn_subdir(callinvoker)
add_rn_subdir(runtimeexecutor)
add_rn_subdir(reactperflogger)
# jsinspector target is defined in jsinspector-modern
add_rn_subdir(jsinspector-modern)

# Platform compat
add_rn_subdir(oscompat)

# Missing deps
add_rn_subdir(jsiexecutor)
add_rn_subdir(jsitooling)
add_rn_subdir(jserrorhandler)
# add_rn_subdir(react/performance) # No CMakeLists.txt

# React (Fabric) - Utils
add_rn_subdir(react/debug)
add_rn_subdir(react/utils)
add_rn_subdir(react/featureflags)
add_rn_subdir(react/timing)
add_rn_subdir(react/runtime)
target_link_libraries(bridgeless callinvoker) # Fix missing link (plain)

# add_rn_subdir(react/bridging) # Bridging might require other stuff?

# Fabric Renderer Core
add_rn_subdir(react/renderer/debug)
add_rn_subdir(react/renderer/graphics)
add_rn_subdir(react/renderer/mapbuffer)
add_rn_subdir(react/renderer/runtimescheduler)
add_rn_subdir(react/renderer/leakchecker)
add_rn_subdir(react/renderer/telemetry)
add_rn_subdir(react/renderer/uimanager)
add_rn_subdir(react/renderer/mounting)
add_rn_subdir(react/renderer/core)
add_rn_subdir(react/renderer/scheduler)
add_rn_subdir(react/renderer/attributedstring)
add_rn_subdir(react/renderer/textlayoutmanager)
add_rn_subdir(react/renderer/imagemanager)
add_rn_subdir(react/renderer/componentregistry)

# Components (Base)
add_rn_subdir(react/renderer/components/view)
add_rn_subdir(react/renderer/components/root)
add_rn_subdir(react/renderer/components/text)
add_rn_subdir(react/renderer/components/image)
add_rn_subdir(react/renderer/components/scrollview)
add_rn_subdir(react/renderer/components/textinput)
add_rn_subdir(react/renderer/components/legacyviewmanagerinterop)

# JSCRuntime (Adapter for system JSC)
add_library(jsc_runtime OBJECT "${REACT_COMMON_DIR}/jsc/JSCRuntime.cpp")
target_include_directories(jsc_runtime PUBLIC "${REACT_COMMON_DIR}/jsc")
target_link_libraries(jsc_runtime jsi folly_runtime glog)
target_compile_options(jsc_runtime PRIVATE -Wno-documentation)

# Create Master Static Library
add_library(react_native_core STATIC dummy.cpp
    $<TARGET_OBJECTS:jsc_runtime>
    $<TARGET_OBJECTS:react_cxxreact>
    $<TARGET_OBJECTS:logger>
    $<TARGET_OBJECTS:runtimeexecutor>
    $<TARGET_OBJECTS:reactperflogger>
    $<TARGET_OBJECTS:jsinspector>
    $<TARGET_OBJECTS:oscompat>
    $<TARGET_OBJECTS:jsireact>
    $<TARGET_OBJECTS:jsitooling>
    $<TARGET_OBJECTS:jserrorhandler>
    $<TARGET_OBJECTS:react_debug>
    $<TARGET_OBJECTS:react_utils>
    $<TARGET_OBJECTS:react_featureflags>
    $<TARGET_OBJECTS:bridgeless>
    $<TARGET_OBJECTS:react_renderer_debug>
    $<TARGET_OBJECTS:react_renderer_graphics>
    $<TARGET_OBJECTS:react_renderer_mapbuffer>
    $<TARGET_OBJECTS:react_renderer_runtimescheduler>
    $<TARGET_OBJECTS:react_renderer_leakchecker>
    $<TARGET_OBJECTS:react_renderer_telemetry>
    $<TARGET_OBJECTS:react_renderer_uimanager>
    $<TARGET_OBJECTS:react_renderer_mounting>
    $<TARGET_OBJECTS:react_renderer_core>
    $<TARGET_OBJECTS:react_renderer_scheduler>
    $<TARGET_OBJECTS:react_renderer_attributedstring>
    $<TARGET_OBJECTS:react_renderer_textlayoutmanager>
    $<TARGET_OBJECTS:react_renderer_imagemanager>
    $<TARGET_OBJECTS:react_renderer_componentregistry>
    $<TARGET_OBJECTS:rrc_view>
    $<TARGET_OBJECTS:rrc_root>
    $<TARGET_OBJECTS:rrc_text>
    $<TARGET_OBJECTS:rrc_image>
    $<TARGET_OBJECTS:rrc_scrollview>
    $<TARGET_OBJECTS:rrc_textinput>
    $<TARGET_OBJECTS:rrc_legacyviewmanagerinterop>
)

target_link_libraries(react_native_core
    PUBLIC
    jsi
    folly_runtime
    glog
    yoga
    callinvoker
    react_timing
    double-conversion
    fmt
)

install(TARGETS react_native_core DESTINATION lib)
