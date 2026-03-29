find_package(glaze CONFIG REQUIRED)
find_package(Utils CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)

add_subdirectory(network)
add_subdirectory(ai)
add_subdirectory(render)
add_subdirectory(phases)
add_subdirectory(game)
add_subdirectory(app)

add_executable(PacManServer)
target_sources(PacManServer PRIVATE main.cpp)
target_link_libraries(PacManServer PRIVATE
    Server::App Server::Game
    glaze::glaze
    Utils::Utils
)
