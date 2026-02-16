add_library(PacManCore STATIC
    Placeholder.cpp
)

target_include_directories(PacManCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(PacManCore
    PUBLIC
        EnTT::EnTT
        spdlog::spdlog
        Utils::Utils
    PRIVATE
        flatbuffers::flatbuffers
        glaze::glaze
)
