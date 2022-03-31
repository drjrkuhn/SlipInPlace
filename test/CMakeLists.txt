cmake_minimum_required(VERSION 3.0.0)
project("test_${CORELIB_NAME}")

find_package(catch2 2 CONFIG REQUIRED)

set(TEST_TARGET ${PROJECT_NAME})
set(TEST_SRCS 
    hrslip.h
    main.cpp	
    test_encode_oop.cpp
    test_encode_ip.cpp
    test_decode_oop.cpp
    test_decode_ip.cpp
    )


add_executable( ${TEST_TARGET}  ${TEST_SRCS})

add_dependencies(${TEST_TARGET}	${CORELIB_NAME})
target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2 ${CORELIB_NAME})

add_executable("devel1" hrslip.h main_devel1.cpp)
add_dependencies("devel1" ${CORELIB_NAME})
target_link_libraries("devel1" PRIVATE ${CORELIB_NAME})

