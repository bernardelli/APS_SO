cmake_minimum_required(VERSION 2.8)
project (bolinha_mask)

find_package( OpenCV REQUIRED )

add_executable(
    bolinha
    ./src/main_bolinha.cpp
)

target_link_libraries(bolinha ${OpenCV_LIBS} )



