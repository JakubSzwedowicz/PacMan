set(GAME_MESSAGES_HANDLER_SOURCES
        ReqRespHandler.cpp
)

add_library(GameMessagesHandlers
        ${GAME_MESSAGES_HANDLER_SOURCES}
)

target_compile_features(GameMessagesHandlers PUBLIC cxx_std_17)
target_include_directories(GameMessagesHandlers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameMessagesHandlers PUBLIC cppzmq Server GameMessages Utils)