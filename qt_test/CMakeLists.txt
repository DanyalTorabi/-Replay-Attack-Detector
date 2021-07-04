cmake_minimum_required(VERSION 2.8)

project(test_replay_packet_detection)
set(LIB_INCLUDE_DIR "../include")
set(INCLUDE_DIR "./include")
set(LIB_SRC_DIR "../src")
set(SRC_DIR "./src")

INCLUDE_DIRECTORIES("${LIB_INCLUDE_DIR} ${INCLUDE_DIR}")
file(GLOB SRC
    "${LIB_INCLUDE_DIR}/*.h"
    "${SRC_DIR}/*.cpp"
    "${INCLUDE_DIR}/*.h"
    "${LIB_SRC_DIR}/*.cpp"
)
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIB_INCLUDE_DIR} ${INCLUDE_DIR})
