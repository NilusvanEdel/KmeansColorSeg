cmake_minimum_required(VERSION 3.3)
project(naiveApproachKMeansCSeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
find_package( OpenCV REQUIRED )
set(SOURCE_FILES Main.cpp VideoCapturer.hpp VideoCapturer.cpp KMeansClus.cpp KMeansClus.hpp Calculator.hpp EuclidianCalculator.cpp EuclidianCalculator.hpp HSVEucCalculator.cpp HSVEucCalculator.hpp CosineCalculator.cpp CosineCalculator.hpp)

add_executable(naiveApproachKMeansCSeg ${SOURCE_FILES})

target_link_libraries( naiveApproachKMeansCSeg ${OpenCV_LIBS} )