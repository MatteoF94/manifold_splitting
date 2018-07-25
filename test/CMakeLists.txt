cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)
#list(APPEND CMAKE_PREFIX_PATH "/opt/ros/kinetic/")
find_package(OpenCV REQUIRED)

include_directories(
        ${OpenCV_INCLUDE_DIRS}
)

add_executable(KLabelPartitionerTest KLabelPartitionerTest.cpp)

target_link_libraries(KLabelPartitionerTest
        ${OpenCV_LIBS}
        )