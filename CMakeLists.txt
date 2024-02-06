cmake_minimum_required(VERSION 3.27)
project(ExParserR)

set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR src)

include_directories(${SRC_DIR})

add_compile_options("-W")
add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-Wpedantic")

add_executable(ExParserR
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/parser/dfa.cpp
    ${SRC_DIR}/parser/grammar.cpp
    ${SRC_DIR}/parser/item.cpp
    ${SRC_DIR}/parser/item_set.cpp
    ${SRC_DIR}/parser/parser.cpp
    ${SRC_DIR}/parser/symbol.cpp
    ${SRC_DIR}/simple_lexer/lexer.cpp
)
