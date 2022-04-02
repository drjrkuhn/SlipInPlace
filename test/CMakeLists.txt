cmake_minimum_required(VERSION 3.8.0)
project("test_${CORELIB_NAME}")

find_package(catch2 2 CONFIG REQUIRED)


set(TEST_TARGET ${PROJECT_NAME})
set(TEST_SRCS 
    hrslip.h
    main.cpp	
    test_encode_oop.cpp
    test_encode_ip.cpp
    test_encode_null.cpp
    test_encode_slip.cpp
    test_decode.cpp
    test_decode_oop.cpp
    test_decode_ip.cpp
    test_decode_null.cpp
    test_decode_slip.cpp
    )


add_executable(${TEST_TARGET}  ${TEST_SRCS})
target_compile_features(${TEST_TARGET} PUBLIC cxx_std_11)
add_dependencies(${TEST_TARGET}	${CORELIB_NAME})
target_link_libraries(${TEST_TARGET} PRIVATE Catch2::Catch2 ${CORELIB_NAME})

add_executable("devel1" hrslip.h main_devel1.cpp)
target_compile_features("devel1" PUBLIC cxx_std_11)
add_dependencies("devel1" ${CORELIB_NAME})
target_link_libraries("devel1" PRIVATE ${CORELIB_NAME})

add_executable("devel2" hrslip.h main_devel2.cpp)
target_compile_features("devel2" PUBLIC cxx_std_11)
add_dependencies("devel2" ${CORELIB_NAME})
target_link_libraries("devel2" PRIVATE ${CORELIB_NAME})

