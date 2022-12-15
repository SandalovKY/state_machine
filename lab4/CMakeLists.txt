cmake_minimum_required(VERSION 3.0.0)
project(lab4 VERSION 0.1.0)

include(CTest)
enable_testing()

add_compile_options(-g)

add_executable(${PROJECT_NAME} main.cpp lab4.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDART 20)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)