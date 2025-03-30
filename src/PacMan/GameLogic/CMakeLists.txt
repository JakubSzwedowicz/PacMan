set(GAME_LOGIC_SOURCES
    GameHandler.cpp
    GameRunner.cpp
)

add_library(GameLogic
    ${GAME_LOGIC_SOURCES}
)

add_subdirectory(Strategies)
target_compile_features(GameLogic PUBLIC cxx_std_17)
target_include_directories(GameLogic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameLogic PUBLIC GameObjects Entities Utils GameEventsManager Strategies)
target_compile_definitions(GameLogic PUBLIC RESOURCES_DIR="${RESOURCES_DESTINATION_PATH}")
