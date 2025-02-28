if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

add_executable(App main.cpp)

target_link_libraries(App PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32> Calc)

add_subdirectory(Calc)
add_subdirectory(PacMan)

target_link_libraries(App PRIVATE PacMan)