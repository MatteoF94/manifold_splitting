cmake_minimum_required(VERSION 3.10)
project(manifold_splitting)

set(CMAKE_CXX_STANDARD 14)
find_package(CGAL REQUIRED)

include_directories(
        include
        ${CGAL_INCLUDE_DIRS}
)

add_executable(manifold_reconstruction
    include/InputManager.h
    include/types_reconstructor.hpp
    src/InputManager.cpp
    main.cpp)

target_link_libraries(manifold_reconstruction
        ${CGAL_LIBRARY}
        )