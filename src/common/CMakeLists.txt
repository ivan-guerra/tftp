cmake_minimum_required(VERSION 3.16)

project(
  common
  DESCRIPTION "shared types and utility functions"
  LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE translate.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${TFTP_INCLUDE_DIR})
