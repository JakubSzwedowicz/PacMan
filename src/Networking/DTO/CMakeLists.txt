find_package(Protobuf REQUIRED)

set(PROTO_GAME_MESSAGES_FILES "GameMessages.proto")
set(PROTO_GAME_MESSAGES_INCLUDES_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(PROTO_GAME_MESSAGES_GENERATED_DIR "${PROTO_GAME_MESSAGES_INCLUDES_DIR}/DTO/GameMessages")

add_library(DTOGameMessages OBJECT ${PROTO_GAME_MESSAGES_FILES})
target_link_libraries(DTOGameMessages PUBLIC protobuf::libprotobuf)
target_include_directories(DTOGameMessages PUBLIC ${PROTO_GAME_MESSAGES_INCLUDES_DIR})

protobuf_generate(
        TARGET DTOGameMessages
        LANGUAGE cpp
        IMPORT_DIRS "."
        PROTOC_OUT_DIR "${PROTO_GAME_MESSAGES_GENERATED_DIR}"
)
