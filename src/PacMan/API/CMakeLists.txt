set(PACMAN_SOURCES
        PacManAPI.cpp)

add_library(PacMan ${PACMAN_SOURCES})
target_compile_features(PacMan PUBLIC cxx_std_17)

target_include_directories(PacMan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(PacMan
        PUBLIC GameObjects)