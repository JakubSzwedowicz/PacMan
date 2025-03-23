find_package(cppzmq REQUIRED)

add_executable(Server
        main.cpp
        Server.cpp
)

target_compile_features(Server PRIVATE cxx_std_17)
target_include_directories(Server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Server PRIVATE cppzmq Utils)