find_package(EnTT CONFIG REQUIRED)

add_library(ECS INTERFACE)
add_library(Core::ECS ALIAS ECS)
target_link_libraries(ECS INTERFACE Core::Core EnTT::EnTT)

add_subdirectory(systems)
