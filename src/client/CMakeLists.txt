cmake_minimum_required(VERSION 3.28)

project(
  client
  DESCRIPTION "TFTP Client"
  LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE config.cpp cmd.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${TFTP_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC common)
