SET(GAME_OBJECTS_SOURCES
        Board.cpp
)

add_library(GameObjects
        ${GAME_OBJECTS_SOURCES})

target_include_directories(GameObjects PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Entities)
target_link_libraries(GameObjects
        PUBLIC Entities
        INTERFACE Entities)

get_target_property(features GameObjects COMPILE_FEATURES)
message(STATUS "GameObjects features: ${features}")
get_target_property(features GameObjects INTERFACE_COMPILE_FEATURES)
message(STATUS "GameObjects interface features: ${features}")