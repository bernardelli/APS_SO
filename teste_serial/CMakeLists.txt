cmake_minimum_required(VERSION 2.8)
project (teste_serial)

#find_package( OpenCV REQUIRED )

include_directories ("${PROJECT_SOURCE_DIR}/src")

set( CMAKE_C_FLAGS  -O3)
add_executable(
    bolinha
    ./src/serial_main.cpp

)



target_link_libraries(bolinha )



