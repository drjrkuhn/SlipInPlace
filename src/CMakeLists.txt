cmake_minimum_required(VERSION 3.8.0)
project(${CORELIB_NAME} VERSION ${CMAKE_PROJECT_VERSION})

add_library(${CORELIB_NAME} INTERFACE SlipInPlace.h SlipUtils.h Polyfills/type_traits.h)
target_compile_features(${CORELIB_NAME} INTERFACE cxx_std_11)
target_include_directories(${CORELIB_NAME} INTERFACE .)
set_target_properties(${CORELIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)