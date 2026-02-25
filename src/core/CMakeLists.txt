add_library(pacman_core INTERFACE)
add_library(Core::Core ALIAS pacman_core)
target_include_directories(pacman_core INTERFACE ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(maps)
