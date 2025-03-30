set(PATH_FINDERS_SOURCES
        AStarPathFinder.cpp
)
add_library(PathFinders ${PATH_FINDERS_SOURCES})
target_compile_features(PathFinders PUBLIC cxx_std_17)
target_include_directories(PathFinders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PathFinders PUBLIC GameObjects Entities Utils)


set(STRATEGIES_SOURCES
        GhostStrategies.cpp
)

add_library(Strategies
        ${STRATEGIES_SOURCES}
)

target_compile_features(Strategies PUBLIC cxx_std_17)
target_include_directories(Strategies PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Strategies PUBLIC GameObjects Entities Utils)
target_link_libraries(Strategies PRIVATE PathFinders)
