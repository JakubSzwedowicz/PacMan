//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef PUBSUBMESSAGES_H
#define PUBSUBMESSAGES_H

#include "GameLogic/GameStatus.h"
#include "MessageType.h"

namespace PacMan {
namespace GameLogic {
namespace GameMessages {

struct PubSubGameStateMessage {
  MessageType type = MessageType::GAME_STATE;
  GameStatus status;
};

} // namespace GameMessages
} // namespace GameLogic
} // namespace PacMan
#endif // PUBSUBMESSAGES_H
