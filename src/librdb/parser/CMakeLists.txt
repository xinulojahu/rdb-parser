set(target_name librdb_parser)

add_library(${target_name} STATIC)

include(CompileOptions)
set_compile_options(${target_name})

target_sources(
    ${target_name}
    PRIVATE
        Lexer.cpp
        Lexer.hpp
        Location.hpp
        Parser.cpp
        Parser.hpp
        Script.hpp
        Statements.cpp
        Statements.hpp
        Token.cpp
        Token.hpp
)

target_include_directories(
    ${target_name}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    ${target_name}
)

set(tests_name librdb_test)
add_executable(${tests_name})

set_compile_options(${tests_name})

target_sources(
    ${tests_name}
    PRIVATE
        Tests.cpp
)

target_include_directories(
    ${tests_name}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    ${tests_name}
    PRIVATE
        librdb_parser
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(${tests_name})
