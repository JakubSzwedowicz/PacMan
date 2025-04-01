add_executable(App main.cpp)

add_subdirectory(PacMan)

target_link_libraries(App PRIVATE GameLogic GameObjects Utils)

add_subdirectory(Networking)
add_subdirectory(Server)
add_subdirectory(Client)

# TODO: Remove below!
# Temporary for the project to build the target
target_link_libraries(App PRIVATE GameLogic Services GameEventsManager)