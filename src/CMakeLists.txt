find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(MICRO16_APPLICATION_FILES
    reader.cpp
    reader.hpp
    main.cpp
)

set(MICRO16_CORE_FILES
    sdl_screen.cpp
    sdl_screen.hpp
    micro16.cpp
    micro16.hpp
)

set(MICRO16_TEST_FILES
    tests/catch.hpp
    tests/catch_extensions.hpp
    tests/testing_main.cpp
    tests/test_instructions.cpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${MICRO16_CORE_FILES} ${MICRO16_APPLICATION_FILES} ${MICRO16_TEST_FILES}
)

add_library(micro16_core
    ${MICRO16_CORE_FILES}
)

add_executable(micro16
    ${MICRO16_APPLICATION_FILES}
)

add_executable(micro16_tests
    ${MICRO16_TEST_FILES}
)

target_link_libraries(micro16_core
    PRIVATE
        ${SDL2_LIBRARIES}
        Threads::Threads
)

target_link_libraries(micro16
    PUBLIC
        micro16_core
)

target_link_libraries(micro16_tests
    PUBLIC
        micro16_core
)
