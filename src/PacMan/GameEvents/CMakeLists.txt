add_library(GameEvents INTERFACE
)

target_include_directories(GameEvents INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameEvents INTERFACE GameLogic Entities)

set (GAME_EVENTS_MANAGER_SOURCES
        GameEventsManager.cpp
        BasicGameEventsPublishers.cpp
)

add_library(GameEventsManager ${GAME_EVENTS_MANAGER_SOURCES})
target_compile_options(GameEventsManager PUBLIC -Werror=switch-enum)
target_include_directories(GameEventsManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameEventsManager PUBLIC GameEvents)
