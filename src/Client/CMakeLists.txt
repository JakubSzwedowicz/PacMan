set(CLIENT_SOURCES
        Client.cpp
)
add_library(Client
        ${CLIENT_SOURCES}
)

target_compile_features(Client PRIVATE cxx_std_17)
target_include_directories(Client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Client PRIVATE GameObjects GameLogic Utils)


add_executable(ClientApp
        main.cpp
)

target_compile_features(ClientApp PRIVATE cxx_std_17)
target_include_directories(ClientApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ClientApp PRIVATE Client GameObjects GameLogic Utils)
