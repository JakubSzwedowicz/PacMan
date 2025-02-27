set(ENTITIES_SOURCES
        Entity.cpp
        Food.cpp
        Ghost.cpp
        PacMan.cpp
        SuperFood.cpp
        Wall.cpp)

add_library(Entities ${ENTITIES_SOURCES})


target_link_libraries(Entities PRIVATE PacManConfiguration)
get_target_property(features Entities COMPILE_FEATURES)
message(STATUS "Entities features: ${features}")
get_target_property(interface_features Entities INTERFACE_COMPILE_FEATURES)
message(STATUS "Entities interface features: ${interface_features}")

target_include_directories(Entities PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})