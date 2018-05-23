cmake_minimum_required(VERSION 3.10)
project(src)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -fopenmp -Wall -lopencv_core -lgdal -lopencv_imgproc -lopencv_flann -lopencv_highgui -lopencv_imgcodecs")

#include_directories(/usr/local/include/opencv2)

#link_directories(/usr/local/lib)


add_executable(optimization
    bellford.cpp
    dijkstra.cpp
    DisplayImage.cpp
    distance.cpp
    distance.h
    exploracion.cpp
    exploracion.h
    main.cpp
    structure.h
    tree.cpp
    tree.h
    treenode.h matrix.h)
