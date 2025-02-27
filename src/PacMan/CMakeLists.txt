set(PACMAN_SOURCES
        PacManAPI.cpp)

add_library(PacManConfiguration INTERFACE)
target_compile_features(PacManConfiguration INTERFACE cxx_std_17)
target_compile_options(PacManConfiguration INTERFACE -O3)

add_library(PacMan ${PACMAN_SOURCES})

target_include_directories(PacMan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


get_target_property(features PacMan COMPILE_FEATURES)
message(STATUS "PacMan features before linking: ${features}")
add_subdirectory(GameObjects)
target_link_libraries(PacMan
        PUBLIC GameObjects
        INTERFACE PacManConfiguration)
get_target_property(features PacMan COMPILE_FEATURES)
message(STATUS "PacMan features after linking: ${features}")