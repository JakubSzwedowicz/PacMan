set(ENTITIES_SOURCES
        Entity.cpp
        PacMan.cpp
        Wall.cpp)

add_library(Entities ${ENTITIES_SOURCES})

target_include_directories(Entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})