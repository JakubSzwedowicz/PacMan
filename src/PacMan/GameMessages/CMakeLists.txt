add_library(GAME_MESSAGES INTERFACE)

target_compile_features(GameLogic PUBLIC cxx_std_17)
target_include_directories(GameLogic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameLogic PRIVATE GameObjects Utils)
