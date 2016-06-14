cmake_minimum_required(VERSION 3.3)
project(naiveApproachKMeansCSeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package( OpenCV REQUIRED )
set(SOURCE_FILES Main.cpp VideoCapturer.hpp VideoCapturer.cpp KMeansClus.cpp KMeansClus.hpp)

add_executable(naiveApproachKMeansCSeg ${SOURCE_FILES})

target_link_libraries( naiveApproachKMeansCSeg ${OpenCV_LIBS} )