cmake_minimum_required(VERSION 3.21)

project(Callisto
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ADD_TEST_CALLISTO "if should build test" OFF)

add_subdirectory(library)

if(ADD_TEST_CALLISTO)
    add_subdirectory(test)
endif()

add_library(razer::callisto ALIAS CallistoLib)
