//
// Created by jakubszwedowicz on 3/29/25.
//

#ifndef GAMEMESSAGESSERIALIZER_H
#define GAMEMESSAGESSERIALIZER_H

#include <GameMessages/GameMessages.pb.h>

namespace PacMan {
namespace GameMessages {

class GameMessagesSerializer {

private:
  proto::GameEvent m_gameEvent;
};

} // proto
} // PacMan

#endif //GAMEMESSAGESSERIALIZER_H
