add_executable(App main.cpp)

target_link_libraries(App PRIVATE Calc)

add_subdirectory(Calc)
add_subdirectory(PacMan)

target_link_libraries(App PRIVATE PacMan Utils)