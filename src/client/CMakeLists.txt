add_executable(PacManClient
    main.cpp
)

target_link_libraries(PacManClient
    PRIVATE
        PacManCore
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        SFML::Network
        ImGui-SFML::ImGui-SFML
        unofficial::enet::enet
)
