set(target_name calclib)

add_library(${target_name} STATIC)

include(CompileOptions)
set_compile_options(${target_name})

target_sources(
    ${target_name}
    PRIVATE
        calclib/sum.hpp
        calclib/sum.cpp
)

target_include_directories(
    ${target_name}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${target_name}
)

