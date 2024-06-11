cmake_minimum_required(VERSION 3.28)

set(TESTNAME common_test)

add_executable(${TESTNAME} pack_test.cpp)

target_link_libraries(${TESTNAME} PRIVATE gtest_main common)

gtest_discover_tests(${TESTNAME} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
