cmake_minimum_required(VERSION 2.8)
project (bolinha_simple)

find_package( OpenCV REQUIRED )
find_package( CUDA REQUIRED )
include_directories ("${PROJECT_SOURCE_DIR}/src")

set( CMAKE_C_FLAGS  -O3)
cuda_add_executable(
    bolinha
    ./src/main_bolinha.cpp
    ./src/serial.cpp

)

add_definitions(-std=c++11)



target_link_libraries(bolinha ${OpenCV_LIBS})



