find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(SOURCE_FILES
    sdl_screen.cpp
    sdl_screen.hpp
    micro16.cpp
    micro16.hpp
    main.cpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${SOURCE_FILES}
)

add_executable(
    micro16
    ${SOURCE_FILES}
)

target_link_libraries(
    micro16
    PRIVATE
    ${SDL2_LIBRARIES}
    Threads::Threads
)
