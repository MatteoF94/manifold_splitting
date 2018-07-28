cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)
#find_package(OpenCV REQUIRED)

include_directories(
    "../src" ,
        "../include"
)

add_executable(KLabelPartitionerTest KLabelPartitionerTest.cpp
        ../src/KLabelPartitioner.cpp
        ../src/SpectralClustering/SpectralClustering.cpp
        ../src/SpectralClustering/SpectralClustering.h BalancedPartitionTest.cpp)

add_executable(BalancedPartitionTest
        BalancedPartitionTest.cpp
        ../src/InputManager.cpp
        ../src/GraphParser.cpp
        )

target_link_libraries(KLabelPartitionerTest
        ${OpenCV_LIBS}
        ${CGAL_LIBRARY}
        gmp
        ${Eigen3_LIBRARY}
        )

target_link_libraries(BalancedPartitionTest
        ${CGAL_LIBRARY}
        )