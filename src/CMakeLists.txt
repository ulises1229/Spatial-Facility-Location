cmake_minimum_required(VERSION 3.10)
project(src)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y  -Wall -lopencv_core -lgdal -lopencv_imgproc -lopencv_flann -lopencv_highgui -lopencv_imgcodecs -v")

#-fopenmp

include_directories("/usr/local/include/" "/usr//local/Cellar/opencv/4.3.0_4/include/opencv4/" "/usr/local/include/")


link_directories("/usr/local/lib")


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
