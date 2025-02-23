set(PACMAN_SOURCES
        PacManAPI.cpp)

add_library(PacMan ${PACMAN_SOURCES})
target_compile_features(PacMan PUBLIC cxx_std_17)
target_compile_options(PacMan PRIVATE -O3)

target_include_directories(PacMan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(GameObjects)
target_link_libraries(PacMan PRIVATE GameObjects)
