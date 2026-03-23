add_library(Core INTERFACE)
add_library(Core::Core ALIAS Core)
target_include_directories(Core INTERFACE ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(maps)
add_subdirectory(ecs)
add_subdirectory(simulation)
