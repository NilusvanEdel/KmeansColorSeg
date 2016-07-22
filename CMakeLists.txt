cmake_minimum_required(VERSION 3.3)
project(naiveApproachKMeansCSeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -lboost_system" )

find_package( OpenCV REQUIRED )
find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES Main.cpp VideoCapturer.hpp VideoCapturer.cpp KMeansClus.cpp KMeansClus.hpp Calculator.hpp
        EuclidianCalculator.cpp EuclidianCalculator.hpp Printer.cpp Printer.hpp CoordEucCalculator.cpp CoordEucCalculator.hpp)

add_executable(naiveApproachKMeansCSeg ${SOURCE_FILES})

target_link_libraries( naiveApproachKMeansCSeg ${OpenCV_LIBS} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})