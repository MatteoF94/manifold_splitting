cmake_minimum_required(VERSION 3.10)
project(manifold_splitting)

set(CMAKE_CXX_STANDARD 14)
find_package(CGAL REQUIRED)
find_package(GMP REQUIRED)

include_directories(
        include
        ${CGAL_INCLUDE_DIRS}
)

add_executable(manifold_splitting
    include/InputManager.h
    include/types_reconstructor.hpp
    src/InputManager.cpp
    main.cpp src/GraphParser.cpp include/GraphParser.h src/KLabelPartitioner.cpp include/KLabelPartitioner.h)

target_link_libraries(manifold_splitting
        ${CGAL_LIBRARY}
        gmp
        )