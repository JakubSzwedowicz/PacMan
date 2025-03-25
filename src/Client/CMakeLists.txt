add_executable(ClientApp
        main.cpp
        Client.cpp
)

target_compile_features(ClientApp PRIVATE cxx_std_17)
target_include_directories(ClientApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ClientApp PRIVATE GameObjects GameLogic Utils)