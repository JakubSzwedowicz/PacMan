find_package(glaze CONFIG REQUIRED)
find_package(Utils CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)

add_subdirectory(network)
add_subdirectory(app)
add_subdirectory(ai)
add_subdirectory(render)
add_subdirectory(phases)

add_executable(PacManServer)
target_sources(PacManServer PRIVATE main.cpp)
# Server::App (via Server::Phases) includes LobbyPhase, and Server::Phases links
# Server::App — a mutual static-lib dependency. Resolved by listing Phases twice
# so the linker makes two passes and resolves all symbols.
target_link_libraries(PacManServer PRIVATE
    Server::Phases Server::App Server::Phases
    glaze::glaze
    Utils::Utils
)
