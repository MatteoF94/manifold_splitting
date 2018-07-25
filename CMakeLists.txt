cmake_minimum_required(VERSION 3.10)
project(manifold_splitting)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(test)

list(APPEND CMAKE_PREFIX_PATH "/opt/ros/kinetic/")
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(CGAL REQUIRED)
find_package(GMP REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include)
include_directories(src/SpectralClustering)
include_directories(
                    ${CGAL_INCLUDE_DIRS}
                    ${EIGEN3_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS}
                   )

add_executable(manifold_splitting
        include/GraphParser.h
        include/InputManager.h
        include/KLabelPartitioner.h
        src/SpectralClustering/SpectralClustering.cpp
        src/SpectralClustering/SpectralClustering.h
        src/GraphParser.cpp
        src/InputManager.cpp
        src/KLabelPartitioner.cpp
        main.cpp)

target_link_libraries(manifold_splitting
        ${CGAL_LIBRARY}
        gmp
        ${Eigen3_LIBRARY}
        ${OpenCV_LIBS}
        )
