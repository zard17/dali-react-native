# FindReactNative.cmake
# Locates the manually built React Native static libraries and headers

set(NODE_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/node_modules")
set(RN_DIR "${NODE_MODULES_DIR}/react-native")
set(RN_COMMON_DIR "${RN_DIR}/ReactCommon")
set(DEPS_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party_dependencies/install")

# 1. React Native Common Source Headers (from node_modules)
set(React_INCLUDE_DIRS
    ${RN_COMMON_DIR}
    ${RN_COMMON_DIR}/callinvoker
    ${RN_COMMON_DIR}/react/renderer/core
    ${RN_COMMON_DIR}/react/renderer/mounting
    ${RN_COMMON_DIR}/react/renderer/uimanager
    ${RN_COMMON_DIR}/react/renderer/componentregistry
    ${RN_COMMON_DIR}/react/renderer/components/view
    ${RN_COMMON_DIR}/react/renderer/components/text
    ${RN_COMMON_DIR}/react/renderer/components/image
    ${RN_COMMON_DIR}/react/renderer/graphics
    ${RN_COMMON_DIR}/react/renderer/imagemanager
    ${RN_COMMON_DIR}/react/renderer/debug
    ${RN_COMMON_DIR}/react/utils
    ${RN_COMMON_DIR}/cxxreact
    ${RN_COMMON_DIR}/jsi
    ${RN_COMMON_DIR}/logger
    ${RN_COMMON_DIR}/yoga
    ${RN_COMMON_DIR}/react/renderer/graphics/platform/cxx
    ${RN_COMMON_DIR}/react/renderer/components/view/platform/cxx
)

# 2. Third Party Install Headers (Folly, Glog, Yoga, etc.)
set(React_ThirdParty_INCLUDE_DIRS
    "${DEPS_INSTALL_DIR}/include"
    "${DEPS_INSTALL_DIR}/include/yoga" # Explicitly include yoga dir as well
)

# 3. Import Static Libraries

# Helper macro to import a static library
macro(add_rn_static_lib name lib_path)
    add_library(${name} STATIC IMPORTED)
    set_target_properties(${name} PROPERTIES
        IMPORTED_LOCATION "${lib_path}"
    )
endmacro()

add_rn_static_lib(React::Core "${DEPS_INSTALL_DIR}/lib/libreact_native_core.a")
add_rn_static_lib(Folly::folly "${DEPS_INSTALL_DIR}/lib/libfolly.a")
add_rn_static_lib(glog::glog "${DEPS_INSTALL_DIR}/lib/libglog.a")
add_rn_static_lib(Yoga::yoga "${DEPS_INSTALL_DIR}/lib/libyogacore.a")
add_rn_static_lib(DoubleConversion::double-conversion "${DEPS_INSTALL_DIR}/lib/libdouble-conversion.a")
add_rn_static_lib(Fmt::fmt "${DEPS_INSTALL_DIR}/lib/libfmt.a")
add_rn_static_lib(LibEvent::core "${DEPS_INSTALL_DIR}/lib/libevent_core.a")
add_rn_static_lib(LibEvent::pthreads "${DEPS_INSTALL_DIR}/lib/libevent_pthreads.a") # Folly might need this

# Boost (Interface only mostly, but we have libs if needed)
set(Boost_INCLUDE_DIR "${DEPS_INSTALL_DIR}/include") 
# We might need to link boost libraries if symbols are missing, but usually header-only for RN usage.
# If needed: 
add_rn_static_lib(Boost::context "${DEPS_INSTALL_DIR}/lib/libboost_context.a")
add_rn_static_lib(Boost::filesystem "${DEPS_INSTALL_DIR}/lib/libboost_filesystem.a")
add_rn_static_lib(Boost::program_options "${DEPS_INSTALL_DIR}/lib/libboost_program_options.a")
add_rn_static_lib(Boost::regex "${DEPS_INSTALL_DIR}/lib/libboost_regex.a")
add_rn_static_lib(Boost::system "${DEPS_INSTALL_DIR}/lib/libboost_system.a")
add_rn_static_lib(Boost::thread "${DEPS_INSTALL_DIR}/lib/libboost_thread.a")

# Aggregate libraries to link against
set(React_LIBRARIES
    React::Core
    Folly::folly
    glog::glog
    Yoga::yoga
    DoubleConversion::double-conversion
    Fmt::fmt
    LibEvent::core
    LibEvent::pthreads
    Boost::context
    Boost::filesystem
    Boost::program_options
    Boost::regex
    Boost::system
    Boost::thread
)

message(STATUS "Found React Native Includes: ${React_INCLUDE_DIRS}")
message(STATUS "Found Third Party Includes: ${React_ThirdParty_INCLUDE_DIRS}")
message(STATUS "React Libraries: ${React_LIBRARIES}")
