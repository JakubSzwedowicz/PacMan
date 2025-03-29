set(ENTITIES_SOURCES
        Empty.cpp
        Entity.cpp
        MovingEntity.cpp
        Food.cpp
        Ghost.cpp
        PacMan.cpp
        SuperFood.cpp
        Wall.cpp)

add_library(Entities ${ENTITIES_SOURCES})
target_compile_features(Entities PUBLIC cxx_std_17)

target_include_directories(Entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Entities PUBLIC GameObjects)