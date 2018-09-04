cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    "../src" ,
        "../include"
)

add_executable(KLabelPartitionerTest KLabelPartitionerTest.cpp
        ../src/KLabelPartitioner.cpp
        ../src/SpectralClustering/SpectralClustering.cpp
        ../src/SpectralClustering/SpectralClustering.h)

add_executable(BalancedPartitionTest
        BalancedPartitionTest.cpp
        ../src/InputManager.cpp
        ../src/MeshManager.cpp
        ../src/GraphParser.cpp
        )

add_executable(MultiTreePartitionerTest
        MultiTreePartitionerTest.cpp
        ../src/MultiTreePartitioner.cpp
        ../include/MultiTreePartitioner.h
        ../src/InputManager.cpp
        ../include/InputManager.h
        )

target_link_libraries(KLabelPartitionerTest
        ${OpenCV_LIBS}
        ${CGAL_LIBRARY}
        gmp
        ${Eigen3_LIBRARY}
        )

target_link_libraries(BalancedPartitionTest
        ${CGAL_LIBRARY}
        gmp
        )

target_link_libraries(MultiTreePartitionerTest
        ${CGAL_LIBRARY}
        gmp
        )