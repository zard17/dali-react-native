# FindHermes.cmake
#
# Find the Hermes JavaScript engine
#
# This module defines:
#   HERMES_FOUND        - True if Hermes was found
#   HERMES_INCLUDE_DIRS - Hermes include directories
#   HERMES_LIBRARIES    - Hermes libraries to link against
#   HERMES_FRAMEWORK    - Path to hermesvm.framework (macOS)
#   HERMES_COMPILER     - Path to hermesc compiler (optional)
#
# User can set HERMES_ROOT to specify the installation prefix

# Default search path
if(NOT HERMES_ROOT)
    set(HERMES_ROOT "${CMAKE_SOURCE_DIR}/third_party_dependencies/hermes-install")
endif()

# Find include directory
find_path(HERMES_INCLUDE_DIR
    NAMES hermes/hermes.h
    PATHS
        ${HERMES_ROOT}/include
        /usr/local/include
        /opt/homebrew/include
    NO_DEFAULT_PATH
)

# On macOS, look for framework first
if(APPLE)
    # Find hermesvm.framework
    find_path(HERMES_FRAMEWORK_DIR
        NAMES hermesvm.framework
        PATHS
            ${HERMES_ROOT}/lib
            /Library/Frameworks
            ~/Library/Frameworks
        NO_DEFAULT_PATH
    )

    if(HERMES_FRAMEWORK_DIR)
        set(HERMES_FRAMEWORK "${HERMES_FRAMEWORK_DIR}/hermesvm.framework")
        set(HERMES_LIBRARY "${HERMES_FRAMEWORK}")
    endif()
endif()

# Fallback to static/shared library if framework not found
if(NOT HERMES_LIBRARY)
    find_library(HERMES_LIBRARY
        NAMES hermes libhermes hermesvm
        PATHS
            ${HERMES_ROOT}/lib
            /usr/local/lib
            /opt/homebrew/lib
        NO_DEFAULT_PATH
    )
endif()

# Find hermesc compiler (optional)
find_program(HERMES_COMPILER
    NAMES hermesc
    PATHS
        ${HERMES_ROOT}/bin
        ${CMAKE_SOURCE_DIR}/node_modules/hermes-engine/osx-bin
        /usr/local/bin
        /opt/homebrew/bin
    NO_DEFAULT_PATH
)

# Handle REQUIRED and QUIET arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Hermes
    REQUIRED_VARS HERMES_LIBRARY HERMES_INCLUDE_DIR
    FAIL_MESSAGE "Could not find Hermes. Please run scripts/build_hermes.sh first."
)

if(HERMES_FOUND)
    set(HERMES_INCLUDE_DIRS ${HERMES_INCLUDE_DIR})
    set(HERMES_LIBRARIES ${HERMES_LIBRARY})

    # Create imported target
    if(NOT TARGET Hermes::hermes)
        if(HERMES_FRAMEWORK)
            # For macOS framework
            add_library(Hermes::hermes SHARED IMPORTED)
            set_target_properties(Hermes::hermes PROPERTIES
                IMPORTED_LOCATION "${HERMES_FRAMEWORK}/hermesvm"
                INTERFACE_INCLUDE_DIRECTORIES "${HERMES_INCLUDE_DIR}"
            )
        else()
            add_library(Hermes::hermes UNKNOWN IMPORTED)
            set_target_properties(Hermes::hermes PROPERTIES
                IMPORTED_LOCATION "${HERMES_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${HERMES_INCLUDE_DIR}"
            )
        endif()
    endif()

    message(STATUS "Found Hermes: ${HERMES_LIBRARY}")
    message(STATUS "Hermes include: ${HERMES_INCLUDE_DIR}")
    if(HERMES_FRAMEWORK)
        message(STATUS "Hermes framework: ${HERMES_FRAMEWORK}")
    endif()
    if(HERMES_COMPILER)
        message(STATUS "Hermes compiler: ${HERMES_COMPILER}")
    endif()
endif()

mark_as_advanced(HERMES_INCLUDE_DIR HERMES_LIBRARY HERMES_FRAMEWORK_DIR HERMES_COMPILER)
