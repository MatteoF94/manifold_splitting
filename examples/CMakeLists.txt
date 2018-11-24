cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math")

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

add_executable(MetisPartitioningTest
        ../src/InputManager.cpp
        ../src/MeshManager.cpp
        MetisPartitioningTest.cpp
        )

add_executable(ParMetisPartitioningTest
        ../src/InputManager.cpp
        ../src/MeshManager.cpp
        ParMetisPartitioningTest.cpp
        )

add_executable(ZoltanGeomPartitioningTest
        ../src/InputManager.cpp
        ../src/MeshManager.cpp
        ZoltanGeomPartitioningTest.cpp
        )

add_executable(ZoltanGraphPartitioningTest
        ../src/InputManager.cpp
        ../src/MeshManager.cpp
        ../src/GraphParser.cpp
        ZoltanGraphPartitioningTest.cpp
        )

add_executable(MultiTreePartitionerTest
        MultiTreePartitionerTest.cpp
        ../src/MultiTreePartitioner.cpp
        ../src/InputManager.cpp
        ../src/MultiTreeManager.cpp
        ../src/MTSerialCreator.cpp
        ../include/MTSerialCreator.h
        ../src/MeshManager.cpp
        ../src/MTVisualizer.cpp
        ../src/MTParallelCreator.cpp
        ../include/MTParallelCreator.h
        ../src/RegularGraphsGen.cpp
        ../src/core/multitree/Concatenator.cpp
        ../src/core/multitree/AdoptionHandler.cpp
        ../src/core/multitree/SerialCreator.cpp
        ../src/core/multitree/SerialCreator.h
        ../src/core/multitree/Creator.cpp
        ../src/core/multitree/Creator.h ../src/core/multitree/CreatorManager.cpp ../src/core/multitree/CreatorManager.h ../src/core/multitree/TreeTypes.h ../src/core/multitree/ParallelCreator.cpp ../src/core/multitree/ParallelCreator.h
        ../src/io/InputManager.cpp ../src/io/InputDescriber.cpp ../src/io/InputDescriber.h ../src/core/multitree/DescendantsHandler.cpp ../src/core/multitree/DescendantsHandler.h ../src/core/multitree/Utilities.cpp ../src/core/multitree/Utilities.h)

add_executable(MultiTreePartitionerDFTest
        MultiTreePartitionerDFTest.cpp
        ../src/MultiTreePartitioner.cpp
        ../src/InputManager.cpp
        ../src/MultiTreeManager.cpp
        ../src/MeshManager.cpp
        )

add_executable(RecursiveMTPTest
        RecursiveMTPTest.cpp
        ../src/MultiTreeManager.cpp
        ../src/MeshManager.cpp
        ../src/InputManager.cpp
        ../src/MultiTreePartitioner.cpp)

add_executable(MPITest
        MPITest.cpp
        )

add_executable(GeneralPartitioningTest
        GeneralPartitioningTest.cpp
        ../src/MeshManager.cpp
        ../src/InputManager.cpp
        ../src/KLabelPartitioner.cpp
        ../src/KSizeClustering.cpp
        ../src/SpectralClustering/SpectralClustering.cpp)

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

target_link_libraries(MetisPartitioningTest
        ${CGAL_LIBRARY}
        gmp
        )

target_link_libraries(ParMetisPartitioningTest
        ${CGAL_LIBRARY}
        gmp
        )

target_link_libraries(ZoltanGeomPartitioningTest
        ${CGAL_LIBRARY}
        gmp
        )

target_link_libraries(ZoltanGraphPartitioningTest
        ${CGAL_LIBRARY}
        gmp
        )

target_link_libraries(MultiTreePartitionerTest
        ${CGAL_LIBRARY}
        gmp
        ${OpenMP_CXX_FLAGS}
        ${OPENGL_LIBRARIES}
        glfw
        )

target_link_libraries(MultiTreePartitionerDFTest
        ${CGAL_LIBRARY}
        gmp
        )

target_compile_options(MultiTreePartitionerTest PUBLIC "${OpenMP_CXX_FLAGS}")

target_link_libraries(RecursiveMTPTest
        ${CGAL_LIBRARY}
        gmp
        )

target_link_libraries(GeneralPartitioningTest
        ${OpenCV_LIBS}
        ${CGAL_LIBRARY}
        gmp
        ${Eigen3_LIBRARY}
        )

target_link_libraries(MPITest
        ${MPI_LIBRARIES}
        ${OpenMP_CXX_FLAGS}
        )

target_compile_options(MPITest PUBLIC
        "${OpenMP_CXX_FLAGS}"
        )