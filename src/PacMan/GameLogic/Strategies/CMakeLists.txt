set(STRATEGIES_SOURCES
        GhostStrategies.cpp
)

add_library(Strategies
        ${STRATEGIES_SOURCES}
)

target_compile_features(Strategies PUBLIC cxx_std_17)
target_include_directories(Strategies PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Strategies PUBLIC GameObjects Entities Utils)
