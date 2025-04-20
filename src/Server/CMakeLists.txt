find_package(cppzmq REQUIRED)

add_executable(ServerApp
        main.cpp
)

target_compile_features(ServerApp PRIVATE cxx_std_17)
target_link_libraries(ServerApp PRIVATE Utils)

add_library(Server
        Server.cpp
)

#add_subdirectory(GameMessagesHandlers)
target_compile_features(Server PUBLIC cxx_std_17)
target_include_directories(Server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Server PUBLIC cppzmq GameLogic Utils)

target_link_libraries(ServerApp PRIVATE Server)