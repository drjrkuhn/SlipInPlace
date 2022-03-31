cmake_minimum_required(VERSION 3.0.0)
project(${CMAKE_PROJECT_NAME}-test)

message("test  CORELIB_NAME = ${CORELIB_NAME}")

find_package(catch2 2 CONFIG REQUIRED)

set(TEST_TARGET ${PROJECT_NAME})
set(TEST_SRCS 
    main.cpp	
    test_encode.cpp
    )


add_executable( ${TEST_TARGET}  ${TEST_SRCS})

add_dependencies(${TEST_TARGET}	${CORELIB_NAME})
target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2 ${CORELIB_NAME})

add_executable("devel1" main_devel1.cpp)
add_dependencies("devel1"	${CORELIB_NAME})
target_link_libraries("devel1" PRIVATE ${CORELIB_NAME})

