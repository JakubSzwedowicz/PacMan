#include "core/protocol/PacketCodec.hpp"

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/verifier.h>

#include <cstring>

#include "packets_generated.h"

namespace pacman::core::protocol {

// ── Internal helpers ───────────────────────────────────────────────────────

static const uint8_t *toU8(std::span<const std::byte> data) { return reinterpret_cast<const uint8_t *>(data.data()); }

static std::vector<std::byte> toBytes(flatbuffers::FlatBufferBuilder &b) {
    std::vector<std::byte> out(b.GetSize());
    std::memcpy(out.data(), b.GetBufferPointer(), b.GetSize());
    return out;
}

static proto::Direction fbDir(ecs::Direction d) { return static_cast<proto::Direction>(static_cast<uint8_t>(d)); }

static ecs::Direction cppDir(proto::Direction d) { return static_cast<ecs::Direction>(static_cast<uint8_t>(d)); }

static proto::GhostType fbGhostType(ecs::GhostType t) { return static_cast<proto::GhostType>(static_cast<uint8_t>(t)); }

static ecs::GhostType cppGhostType(proto::GhostType t) { return static_cast<ecs::GhostType>(static_cast<uint8_t>(t)); }

static proto::GhostMode fbGhostMode(uint8_t m) { return static_cast<proto::GhostMode>(m); }

// Build a PlayerInfoFB offset.
static flatbuffers::Offset<proto::PlayerInfoFB> buildPlayerInfo(flatbuffers::FlatBufferBuilder &b,
                                                                const PlayerInfo &pi) {
    auto name = b.CreateString(pi.name);
    return proto::CreatePlayerInfoFB(b, pi.id, name, pi.connected, pi.ready);
}

// Build an EntityStateFB offset.
static flatbuffers::Offset<proto::EntityStateFB> buildEntityState(flatbuffers::FlatBufferBuilder &b,
                                                                  const EntityState &es) {
    auto name = b.CreateString(es.name);
    return proto::CreateEntityStateFB(b, es.id, name, es.x, es.y, fbDir(es.dir), es.score, es.lives, es.alive);
}

// ── getType ────────────────────────────────────────────────────────────────

PacketCodec::PacketType PacketCodec::getType(std::span<const std::byte> data) {
    if (data.size() < 4) return PacketType::Unknown;
    flatbuffers::Verifier v(toU8(data), data.size());
    if (!proto::VerifyPacketBuffer(v)) return PacketType::Unknown;
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return PacketType::Unknown;
    switch (pkt->payload_type()) {
        case proto::AnyPacket_LobbyStatePacketFB:
            return PacketType::LobbyState;
        case proto::AnyPacket_LobbyReadyPacketFB:
            return PacketType::LobbyReady;
        case proto::AnyPacket_GameStartPacketFB:
            return PacketType::GameStart;
        case proto::AnyPacket_ReadyToPlayPacketFB:
            return PacketType::ReadyToPlay;
        case proto::AnyPacket_PlayerInputPacketFB:
            return PacketType::PlayerInput;
        case proto::AnyPacket_GameSnapshotPacketFB:
            return PacketType::GameSnapshot;
        case proto::AnyPacket_RoundEndPacketFB:
            return PacketType::RoundEnd;
        case proto::AnyPacket_ServerShutdownPacketFB:
            return PacketType::ServerShutdown;
        default:
            return PacketType::Unknown;
    }
}

// ── Serialize ──────────────────────────────────────────────────────────────

std::vector<std::byte> PacketCodec::serialize(const LobbyStatePacket &p) {
    flatbuffers::FlatBufferBuilder b;
    std::vector<flatbuffers::Offset<proto::PlayerInfoFB>> players;
    players.reserve(p.players.size());
    for (const auto &pi : p.players) players.push_back(buildPlayerInfo(b, pi));
    auto playersVec = b.CreateVector(players);
    auto payload = proto::CreateLobbyStatePacketFB(b, playersVec);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_LobbyStatePacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const LobbyReadyPacket &p) {
    flatbuffers::FlatBufferBuilder b;
    auto payload = proto::CreateLobbyReadyPacketFB(b, p.playerId, p.ready);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_LobbyReadyPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const GameStartPacket &p) {
    flatbuffers::FlatBufferBuilder b;
    auto mapJson = b.CreateString(p.mapJson);

    std::vector<flatbuffers::Offset<proto::TileFB>> spawns;
    spawns.reserve(p.spawnPositions.size());
    for (const auto &t : p.spawnPositions)
        spawns.push_back(proto::CreateTileFB(b, static_cast<uint64_t>(t.col()), static_cast<uint64_t>(t.row())));
    auto spawnsVec = b.CreateVector(spawns);

    std::vector<uint32_t> ids(p.playerIds.begin(), p.playerIds.end());
    auto idsVec = b.CreateVector(ids);

    auto payload = proto::CreateGameStartPacketFB(b, mapJson, spawnsVec, idsVec, p.assignedPlayerId);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_GameStartPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const ReadyToPlayPacket &p) {
    flatbuffers::FlatBufferBuilder b;
    auto payload = proto::CreateReadyToPlayPacketFB(b, p.playerId);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_ReadyToPlayPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const PlayerInputPacket &p) {
    flatbuffers::FlatBufferBuilder b;
    auto payload = proto::CreatePlayerInputPacketFB(b, p.tick, p.playerId, fbDir(p.dir));
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_PlayerInputPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const GameSnapshotPacket &p) {
    flatbuffers::FlatBufferBuilder b;

    std::vector<flatbuffers::Offset<proto::EntityStateFB>> players;
    players.reserve(p.players.size());
    for (const auto &es : p.players) players.push_back(buildEntityState(b, es));
    auto playersVec = b.CreateVector(players);

    std::vector<flatbuffers::Offset<proto::GhostStateFB>> ghosts;
    ghosts.reserve(ghostCount);
    for (int i = 0; i < ghostCount; ++i) {
        const auto &g = p.ghosts[i];
        ghosts.push_back(
            proto::CreateGhostStateFB(b, fbGhostType(g.type), g.x, g.y, fbDir(g.dir), fbGhostMode(g.mode)));
    }
    auto ghostsVec = b.CreateVector(ghosts);

    std::vector<flatbuffers::Offset<proto::TileFB>> pellets;
    pellets.reserve(p.remainingPellets.size());
    for (const auto &t : p.remainingPellets)
        pellets.push_back(proto::CreateTileFB(b, static_cast<uint64_t>(t.col()), static_cast<uint64_t>(t.row())));
    auto pelletsVec = b.CreateVector(pellets);

    std::vector<flatbuffers::Offset<proto::TileFB>> powerPellets;
    powerPellets.reserve(p.remainingPowerPellets.size());
    for (const auto &t : p.remainingPowerPellets)
        powerPellets.push_back(proto::CreateTileFB(b, static_cast<uint64_t>(t.col()), static_cast<uint64_t>(t.row())));
    auto powerPelletsVec = b.CreateVector(powerPellets);

    auto payload = proto::CreateGameSnapshotPacketFB(b, p.tick, playersVec, ghostsVec, pelletsVec, powerPelletsVec);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_GameSnapshotPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const RoundEndPacket &p) {
    flatbuffers::FlatBufferBuilder b;
    std::vector<flatbuffers::Offset<proto::EntityStateFB>> scores;
    scores.reserve(p.finalScores.size());
    for (const auto &es : p.finalScores) scores.push_back(buildEntityState(b, es));
    auto scoresVec = b.CreateVector(scores);
    auto payload = proto::CreateRoundEndPacketFB(b, scoresVec, p.winnerId);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_RoundEndPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

std::vector<std::byte> PacketCodec::serialize(const ServerShutdownPacket &p) {
    flatbuffers::FlatBufferBuilder b;
    auto reason = b.CreateString(p.reason);
    auto payload = proto::CreateServerShutdownPacketFB(b, reason);
    auto pkt = proto::CreatePacket(b, proto::AnyPacket_ServerShutdownPacketFB, payload.Union());
    proto::FinishPacketBuffer(b, pkt);
    return toBytes(b);
}

// ── Deserialize ────────────────────────────────────────────────────────────

std::optional<LobbyReadyPacket> PacketCodec::deserializeLobbyReady(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_LobbyReadyPacketFB();
    if (!fb) return std::nullopt;
    return LobbyReadyPacket{fb->player_id(), fb->ready()};
}

std::optional<ReadyToPlayPacket> PacketCodec::deserializeReadyToPlay(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_ReadyToPlayPacketFB();
    if (!fb) return std::nullopt;
    return ReadyToPlayPacket{fb->player_id()};
}

std::optional<PlayerInputPacket> PacketCodec::deserializePlayerInput(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_PlayerInputPacketFB();
    if (!fb) return std::nullopt;
    return PlayerInputPacket{fb->tick(), fb->player_id(), cppDir(fb->dir())};
}

std::optional<LobbyStatePacket> PacketCodec::deserializeLobbyState(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_LobbyStatePacketFB();
    if (!fb) return std::nullopt;

    LobbyStatePacket out;
    if (fb->players()) {
        out.players.reserve(fb->players()->size());
        for (const auto *pi : *fb->players()) {
            if (!pi) continue;
            out.players.push_back({pi->id(), pi->name() ? pi->name()->str() : "", pi->connected(), pi->ready()});
        }
    }
    return out;
}

std::optional<GameStartPacket> PacketCodec::deserializeGameStart(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_GameStartPacketFB();
    if (!fb) return std::nullopt;

    GameStartPacket out;
    out.mapJson = fb->map_json() ? fb->map_json()->str() : "";
    out.assignedPlayerId = fb->assigned_player_id();

    if (fb->spawn_positions()) {
        out.spawnPositions.reserve(fb->spawn_positions()->size());
        for (const auto *t : *fb->spawn_positions()) {
            if (!t) continue;
            out.spawnPositions.push_back(maps::Tile{{static_cast<size_t>(t->col()), static_cast<size_t>(t->row())}});
        }
    }
    if (fb->player_ids()) {
        out.playerIds.reserve(fb->player_ids()->size());
        for (auto id : *fb->player_ids()) out.playerIds.push_back(id);
    }
    return out;
}

std::optional<GameSnapshotPacket> PacketCodec::deserializeGameSnapshot(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_GameSnapshotPacketFB();
    if (!fb) return std::nullopt;

    GameSnapshotPacket out;
    out.tick = fb->tick();

    if (fb->players()) {
        out.players.reserve(fb->players()->size());
        for (const auto *es : *fb->players()) {
            if (!es) continue;
            out.players.push_back(
                {es->id(),
                 es->name() ? es->name()->str() : "",
                 es->x(),
                 es->y(),
                 cppDir(es->dir()),
                 es->score(),
                 es->lives(),
                 es->alive()});
        }
    }
    if (fb->ghosts()) {
        int n = static_cast<int>(fb->ghosts()->size());
        if (n > ghostCount) n = ghostCount;
        for (int i = 0; i < n; ++i) {
            const auto *gs = (*fb->ghosts())[i];
            if (!gs) continue;
            out.ghosts[i] = {cppGhostType(gs->type()), gs->x(), gs->y(), cppDir(gs->dir()),
                             static_cast<uint8_t>(gs->mode())};
        }
    }
    if (fb->pellets()) {
        out.remainingPellets.reserve(fb->pellets()->size());
        for (const auto *t : *fb->pellets()) {
            if (!t) continue;
            out.remainingPellets.push_back(maps::Tile{{static_cast<size_t>(t->col()), static_cast<size_t>(t->row())}});
        }
    }
    if (fb->power_pellets()) {
        out.remainingPowerPellets.reserve(fb->power_pellets()->size());
        for (const auto *t : *fb->power_pellets()) {
            if (!t) continue;
            out.remainingPowerPellets.push_back(
                maps::Tile{{static_cast<size_t>(t->col()), static_cast<size_t>(t->row())}});
        }
    }
    return out;
}

std::optional<RoundEndPacket> PacketCodec::deserializeRoundEnd(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_RoundEndPacketFB();
    if (!fb) return std::nullopt;

    RoundEndPacket out;
    out.winnerId = fb->winner_id();
    if (fb->final_scores()) {
        out.finalScores.reserve(fb->final_scores()->size());
        for (const auto *es : *fb->final_scores()) {
            if (!es) continue;
            out.finalScores.push_back(
                {es->id(),
                 es->name() ? es->name()->str() : "",
                 es->x(),
                 es->y(),
                 cppDir(es->dir()),
                 es->score(),
                 es->lives(),
                 es->alive()});
        }
    }
    return out;
}

std::optional<ServerShutdownPacket> PacketCodec::deserializeServerShutdown(std::span<const std::byte> data) {
    const auto *pkt = proto::GetPacket(toU8(data));
    if (!pkt) return std::nullopt;
    const auto *fb = pkt->payload_as_ServerShutdownPacketFB();
    if (!fb) return std::nullopt;
    return ServerShutdownPacket{fb->reason() ? fb->reason()->str() : ""};
}

}  // namespace pacman::core::protocol
