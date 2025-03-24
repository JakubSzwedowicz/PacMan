//
// Created by jakubszwedowicz on 3/23/25.
//

#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H
#include <cstdint>

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

enum class MessageSource : uint8_t { CLIENT, SERVER };
enum class MessageType : uint8_t {
  CONNECTION_MESSAGE,
  GAME_MESSAGE,
};

struct MessageHeader {
  MessageSource source;
  MessageType event;
};

struct Payload {
  std::vector<uint8_t> data;
};

} // namespace GameMessages
} // namespace GameLogic
} // namespace PacMan

#endif // MESSAGETYPE_H
