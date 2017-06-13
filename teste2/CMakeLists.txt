cmake_minimum_required(VERSION 2.8)
project (bolinha_cuda_detect)

find_package( OpenCV REQUIRED )




add_executable(
    bolinha_cuda
    ./src/main_bolinha_cuda.cpp
)

target_link_libraries(bolinha_cuda ${OpenCV_LIBS} )
