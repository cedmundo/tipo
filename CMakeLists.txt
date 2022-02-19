cmake_minimum_required(VERSION 3.21)
project(tipo C)

set(CMAKE_C_STANDARD 17)
include(cmake/FetchCMocka.cmake)
add_definitions("-Wall" "-g")

# Sources and headers
set(HEADERS lex.h parse.h)
set(SOURCES lex.c parse.c)

# Executable building
add_executable(tipo ${HEADERS} ${SOURCES} main.c)

# Testing
add_executable(lex_test lex.h lex.c lex_test.c)
target_compile_features(lex_test PRIVATE c_std_99)
target_link_libraries(lex_test PRIVATE cmocka-static)

add_executable(parse_test lex.h lex.c parse.h parse.c parse_test.c)
target_compile_features(parse_test PRIVATE c_std_99)
target_link_libraries(parse_test PRIVATE cmocka-static)

enable_testing()
add_test(NAME lex_test COMMAND lex_test)
add_test(NAME parse_test COMMAND parse_test)