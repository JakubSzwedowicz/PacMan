find_package(spdlog REQUIRED)

set(UTILS_SOURCES
        ILogger.cpp
        Logger.cpp
)

add_library(Utils
        ${UTILS_SOURCES})

set_target_properties(Utils PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(Utils PUBLIC cxx_std_17)

target_link_libraries(Utils PUBLIC spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
