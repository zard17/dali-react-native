cmake_minimum_required(VERSION 3.16)
project(dali-native-benchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use same DALi paths as the main project
set(DALI_ENV_PATH "$ENV{HOME}/tizen/dali-env")

add_executable(dali-native-app main.cpp)

target_include_directories(dali-native-app PRIVATE
    ${DALI_ENV_PATH}/include
    ${DALI_ENV_PATH}/include/dali
)

target_link_directories(dali-native-app PRIVATE
    ${DALI_ENV_PATH}/lib
)

target_link_libraries(dali-native-app PRIVATE
    dali2-core
    dali2-adaptor
    dali2-toolkit
)
