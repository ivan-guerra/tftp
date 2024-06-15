cmake_minimum_required(VERSION 3.28)

set(TESTNAME client_test)

add_executable(${TESTNAME} config_test.cpp cmd_parse_test.cpp)

target_link_libraries(${TESTNAME} PRIVATE gtest_main client)

gtest_discover_tests(${TESTNAME} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests)

set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
