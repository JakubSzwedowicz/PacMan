cmake_minimum_required(VERSION 3.10)

project(PacMan LANGUAGES CXX)

enable_testing()

add_subdirectory(src)

# TODO: Add it back if I write some tests
add_subdirectory(test)