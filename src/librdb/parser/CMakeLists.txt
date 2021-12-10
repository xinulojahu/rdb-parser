set(target_name librdb_lexer)

add_library(${target_name} STATIC)

include(CompileOptions)
set_compile_options(${target_name})

target_sources(
    ${target_name}
    PRIVATE
        Lexer.cpp
        Lexer.hpp
        Location.hpp
        Token.cpp
        Token.hpp
)

target_include_directories(
    ${target_name}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${target_name}
)

set(tests_name librdb_lexertest)
add_executable(${tests_name})

set_compile_options(${tests_name})

target_sources(
    ${tests_name}
    PRIVATE
        LexerTests.cpp
)

target_include_directories(
    ${tests_name}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    ${tests_name}
    PRIVATE
        librdb_lexer
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(${tests_name})
