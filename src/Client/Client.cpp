//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include "Client/Client.h"
#include "Entities/Ghost.h"
#include "Entities/PacMan.h"
#include "GameLogic/GameRunner.h"
#include "GameObjects/Level.h"

namespace PacMan {
namespace Client {

Client::Client(GameEvents::GameEventsManager &gameEventsManager) : m_ {}

int Client::main() {}

void Client::startSinglePlayerGame() { const auto boardName = "Board1.txt"; }

} // namespace Client
} // namespace PacMan