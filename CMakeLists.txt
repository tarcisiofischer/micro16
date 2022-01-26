cmake_minimum_required(VERSION 3.16)
project(micro16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

add_executable(micro16
    sdl_screen.cpp
    sdl_screen.hpp
    micro16.cpp
    micro16.hpp
    main.cpp
)

message(STATUS ${SDL2_LIBRARIES})

target_link_libraries(
    micro16
    PRIVATE
    ${SDL2_LIBRARIES}
    Threads::Threads
)
