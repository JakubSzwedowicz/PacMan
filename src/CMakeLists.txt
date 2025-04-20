add_executable(App main.cpp)

add_subdirectory(PacMan)

target_link_libraries(App PRIVATE GameEventsManager Utils)

add_subdirectory(Networking)
add_subdirectory(Server)
add_subdirectory(Client)