set(GAME_OBJECTS_SOURCES
        Level.cpp
        LevelBuilderFromFile.cpp
)

add_library(GameObjects
        ${GAME_OBJECTS_SOURCES})

target_include_directories(GameObjects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Entities)

target_link_libraries(GameObjects
        PUBLIC Entities)
