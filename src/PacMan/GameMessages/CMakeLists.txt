add_subdirectory(proto)

add_library(GameMessagesSerializer
        GameMessagesSerializer.cpp
)

target_link_libraries(GameMessagesSerializer PUBLIC ProtoGameMessages)