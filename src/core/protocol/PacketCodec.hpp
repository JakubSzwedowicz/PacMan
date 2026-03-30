#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "core/protocol/Packets.hpp"

// PacketCodec — stateless serialize/deserialize helpers.
//
// Serialize:  in-memory Packet struct  →  FlatBuffers bytes (vector<byte>)
// Deserialize: FlatBuffers bytes       →  optional<in-memory Packet struct>
//
// packets_generated.h is NOT included here; it's an impl detail in PacketCodec.cpp.
// Consumers only need Packets.hpp.

namespace pacman::core::protocol {

class PacketCodec {
   public:
    // Discriminator read from the wire without full deserialization.
    enum class PacketType : uint8_t {
        Unknown = 0,
        LobbyState,
        LobbyReady,
        GameStart,
        ReadyToPlay,
        PlayerInput,
        GameSnapshot,
        RoundEnd,
        ServerShutdown,
    };

    [[nodiscard]] static PacketType getType(std::span<const std::byte> data);

    [[nodiscard]] static std::vector<std::byte> serialize(const LobbyStatePacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const LobbyReadyPacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const GameStartPacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const ReadyToPlayPacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const PlayerInputPacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const GameSnapshotPacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const RoundEndPacket& p);
    [[nodiscard]] static std::vector<std::byte> serialize(const ServerShutdownPacket& p);

    // client → server
    [[nodiscard]] static std::optional<LobbyReadyPacket> deserializeLobbyReady(std::span<const std::byte> data);
    [[nodiscard]] static std::optional<ReadyToPlayPacket> deserializeReadyToPlay(std::span<const std::byte> data);
    [[nodiscard]] static std::optional<PlayerInputPacket> deserializePlayerInput(std::span<const std::byte> data);

    // server → client
    [[nodiscard]] static std::optional<LobbyStatePacket> deserializeLobbyState(std::span<const std::byte> data);
    [[nodiscard]] static std::optional<GameStartPacket> deserializeGameStart(std::span<const std::byte> data);
    [[nodiscard]] static std::optional<GameSnapshotPacket> deserializeGameSnapshot(std::span<const std::byte> data);
    [[nodiscard]] static std::optional<RoundEndPacket> deserializeRoundEnd(std::span<const std::byte> data);
    [[nodiscard]] static std::optional<ServerShutdownPacket> deserializeServerShutdown(std::span<const std::byte> data);
};

}  // namespace pacman::core::protocol
