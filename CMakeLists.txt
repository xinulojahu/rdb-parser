cmake_minimum_required(VERSION 3.21.2)

project(
    Calc   
    VERSION 1.0
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

add_subdirectory(external)
add_subdirectory(src)
