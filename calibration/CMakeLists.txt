cmake_minimum_required(VERSION 2.8)
project (calibrate)

find_package( OpenCV REQUIRED )

set(CMAKE_BUILD_TYPE Debug)

add_executable(
    calibrate
    ./src/camera_calibration.cpp

)

target_link_libraries(calibrate ${OpenCV_LIBS} )



