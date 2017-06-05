cmake_minimum_required(VERSION 2.8)
project (TomoSimulator)

find_package(OPENCV REQUIRED)


add_executable(
    bolinha
    ./src/main_bolinha.cpp
)

target_link_libraries(bolinha opencv)