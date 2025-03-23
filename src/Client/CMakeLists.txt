add_executable(Client
        Client.cpp
)

target_compile_features(Client PRIVATE cxx_std_17)
target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Client PRIVATE GameObjects, Utils)