add_library(ECSSystems STATIC)
add_library(Core::ECSSystems ALIAS ECSSystems)
target_sources(ECSSystems PRIVATE MovementSystem.cpp CollisionSystem.cpp)
target_link_libraries(ECSSystems
    PUBLIC Core::ECS Core::Maps
)
