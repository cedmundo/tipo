cmake_minimum_required(VERSION 3.21)
project(tipo C)

set(CMAKE_C_STANDARD 17)
include(cmake/FetchCMocka.cmake)
add_definitions("-Wall" "-g")

# Sources and headers
set(HEADERS utf8.h lex.h parse.h scope.h reduce.h ir.h)
set(SOURCES utf8.c lex.c parse.c scope.c reduce.c ir.c)

# Executable building
add_executable(tipo ${HEADERS} ${SOURCES} main.c)

# Testing
add_executable(utf8_test utf8.h utf8.c utf8_test.c)
target_compile_features(utf8_test PRIVATE c_std_99)
target_link_libraries(utf8_test PRIVATE cmocka-static)

add_executable(lex_test  utf8.h utf8.c lex.h lex.c lex_test.c)
target_compile_features(lex_test PRIVATE c_std_99)
target_link_libraries(lex_test PRIVATE cmocka-static)

add_executable(parse_test utf8.h utf8.c lex.h lex.c parse.h parse.c parse_test.c)
target_compile_features(parse_test PRIVATE c_std_99)
target_link_libraries(parse_test PRIVATE cmocka-static)

add_executable(ir_test utf8.h utf8.c lex.h lex.c parse.h parse.c reduce.h reduce.c ir.h ir.c ir_test.c)
target_compile_features(ir_test PRIVATE c_std_99)
target_link_libraries(ir_test PRIVATE cmocka-static)

add_executable(scope_test utf8.h utf8.c lex.h lex.c parse.h parse.c ir.h ir.c scope.h scope.c scope_test.c)
target_compile_features(scope_test PRIVATE c_std_99)
target_link_libraries(scope_test PRIVATE cmocka-static)

add_executable(reduce_test utf8.h utf8.c lex.h lex.c parse.h parse.c ir.h ir.c scope.h scope.c reduce.h reduce.c reduce_test.c)
target_compile_features(reduce_test PRIVATE c_std_99)
target_link_libraries(reduce_test PRIVATE cmocka-static)

enable_testing()
add_test(NAME utf8_test COMMAND utf8_test)
add_test(NAME lex_test COMMAND lex_test)
add_test(NAME parse_test COMMAND parse_test)
add_test(NAME ir_test COMMAND ir_test)
add_test(NAME scope_test COMMAND scope_test)
add_test(NAME reduce_test COMMAND reduce_test)
