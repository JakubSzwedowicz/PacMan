find_package(Protobuf REQUIRED)

set(PROTO_GAME_MESSAGES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/GameMessages.proto")
set(PROTO_GAME_MESSAGES_INCLUDES_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated/")
set(PROTO_GAME_MESSAGES_GENERATED_DIR "${PROTO_GAME_MESSAGES_INCLUDES_DIR}/GameMessages")

add_library(ProtoGameMessages OBJECT ${PROTO_GAME_MESSAGES_FILES})
target_link_libraries(ProtoGameMessages PUBLIC protobuf::libprotobuf)
target_include_directories(ProtoGameMessages PUBLIC ${PROTO_GAME_MESSAGES_INCLUDES_DIR})

protobuf_generate(
        TARGET ProtoGameMessages
        LANGUAGE cpp
        IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
        PROTOC_OUT_DIR "${PROTO_GAME_MESSAGES_GENERATED_DIR}"
)
