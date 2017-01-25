cmake_minimum_required(VERSION 2.8)

project(ssp-tests)

find_package (GTest REQUIRED)

include_directories (
    ${GTEST_INCLUDE_DIR}
    ../ssp/Inc
    ../ssp
)

set(EXE_SOURCES
    ssp-internal-test.cpp
    ssp-driver-stub.cpp
    ../ssp/src/ssp-master.c
    ../ssp/src/ssp-slave.c
    ../ssp/src/ssp-utils.c
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
)

add_test(NAME Library_testing
         COMMAND ${PROJECT_NAME})

run_test(${PROJECT_NAME})
