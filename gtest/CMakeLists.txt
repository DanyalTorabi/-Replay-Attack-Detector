cmake_minimum_required(VERSION 2.8)

project(GTest-Replay-Packet-Detection)
INCLUDE_DIRECTORIES("../include")
enable_testing()
find_package(GTest REQUIRED)
add_executable(runUnitTests
  main.cpp ../src/replay_detector.cpp ../include/replay_detector.h
)

target_link_libraries(runUnitTests gtest gtest_main pthread)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)
