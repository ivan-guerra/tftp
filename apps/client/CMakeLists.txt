cmake_minimum_required(VERSION 3.28)

project(
  tftpc
  DESCRIPTION "TFTP Client"
  LANGUAGES CXX)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE client.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${TFTP_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC common)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
