//
// Created by Jakub Szwedowicz on 3/23/25.
//

#ifndef SERVER_H
#define SERVER_H

#include <string>

class Server {
public:
  void listenForPlayers();
  void startGame();

private:
  const std::string m_clientServerReqReplSocketAddr = "tcp://*:5555";
};

#endif // SERVER_H
