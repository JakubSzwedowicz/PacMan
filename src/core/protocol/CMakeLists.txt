find_package(flatbuffers CONFIG REQUIRED)

set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Generate C++ headers from packets.fbs using flatc.
# build_flatbuffers() is provided by the flatbuffers CMake config.
build_flatbuffers(
    "${CMAKE_CURRENT_SOURCE_DIR}/packets.fbs"  # schema(s)
    ""                                          # include dirs
    "GenerateProtocol"                          # custom target name
    ""                                          # additional deps
    "${PROTO_GEN_DIR}"                          # generated headers dir → packets_generated.h
    ""                                          # binary schemas dir (unused)
    ""                                          # copy text schemas dir (unused)
)

add_library(Protocol STATIC)
add_library(Core::Protocol ALIAS Protocol)
add_dependencies(Protocol GenerateProtocol)
target_sources(Protocol PRIVATE PacketCodec.cpp)
target_include_directories(Protocol PUBLIC "${PROTO_GEN_DIR}")
target_link_libraries(Protocol
    PUBLIC  Core::Core flatbuffers::flatbuffers
)
