cmake_minimum_required(VERSION 2.8)
project (bolinha_cuda_mask)

find_package( OpenCV REQUIRED )
find_package( CUDA REQUIRED )
include_directories ("${PROJECT_SOURCE_DIR}/src")


cuda_add_executable(
    bolinha
    ./src/main_bolinha.cpp
    ./src/inRange.cu
    ./src/inRange.cuh
)


target_link_libraries(bolinha ${OpenCV_LIBS} ${CUDA_LIBRARIES})



