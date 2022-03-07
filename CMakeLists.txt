cmake_minimum_required(VERSION 3.0.0)
project(SonarSimulation VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_link_options(-lpthread)

add_executable(SonarSimulation src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
