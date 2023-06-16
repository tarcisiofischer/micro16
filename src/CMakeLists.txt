find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
include(CTest)

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

set(MICRO16_ASSEMBLER_LIB_FILES
    assembler/lexer.hpp
    assembler/lexer.cpp
    assembler/parser.hpp
    assembler/parser.cpp
)

set(MICRO16_ASSEMBLER_CLI_FILES
    assembler/main.cpp
)

set(MICRO16_TEST_FILES
    tests/catch.hpp
    tests/catch_extensions.hpp
    tests/testing_main.cpp
    tests/test_instructions.cpp
    tests/test_assembler.cpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    PREFIX "Source Files"
    FILES ${MICRO16_CORE_FILES} ${MICRO16_APPLICATION_FILES} ${MICRO16_ASSEMBLER_LIB_FILES} ${MICRO16_ASSEMBLER_CLI_FILES} ${MICRO16_TEST_FILES}
)

add_library(micro16_core
    ${MICRO16_CORE_FILES}
)
target_link_libraries(micro16_core
    PRIVATE
    ${SDL2_LIBRARIES}
    Threads::Threads
)

add_executable(micro16
    ${MICRO16_APPLICATION_FILES}
)
target_link_libraries(micro16
    PUBLIC
        micro16_core
)

add_library(micro16_assembler_lib
    ${MICRO16_ASSEMBLER_LIB_FILES}
)
add_executable(micro16_asm
    ${MICRO16_ASSEMBLER_CLI_FILES}
)
target_link_libraries(micro16_asm
    PUBLIC
    micro16_assembler_lib
)

add_executable(micro16_tests
    ${MICRO16_TEST_FILES}
)
target_link_libraries(micro16_tests
    PUBLIC
    micro16_core
    micro16_assembler_lib
)
add_test(NAME micro16_tests COMMAND micro16_tests)
add_custom_command(
    TARGET micro16_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/tests/test_assembler/
    ${CMAKE_CURRENT_BINARY_DIR}/test_assembler/
)
