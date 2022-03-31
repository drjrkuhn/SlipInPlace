cmake_minimum_required(VERSION 3.0.0)
project(${CORELIB_NAME} VERSION 0.1.0)

add_library(${CORELIB_NAME} INTERFACE)
target_include_directories(${CORELIB_NAME} INTERFACE .)
set_target_properties(${CORELIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)