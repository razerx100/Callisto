cmake_minimum_required(VERSION 3.21)

project(Callisto
    LANGUAGES CXX
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    add_definitions(-std=c++20)
else()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

option(ADD_TEST_CALLISTO "If test should be built" OFF)

add_subdirectory(library)

if(ADD_TEST_CALLISTO)
    enable_testing()
    add_subdirectory(test)
endif()

add_library(razer::callisto ALIAS CallistoLib)
