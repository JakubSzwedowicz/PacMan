//
// Created by Jakub Szwedowicz on 3/23/25.
//

#include <chrono>
#include <iostream>
#include <thread>
#include <zmq.hpp>

#include "Server.h"

void Server::startGame() {
  zmq::context_t context(1);
  zmq::socket_t socket(context, ZMQ_REP);
  socket.bind("tcp://*:5555"); // Listen for requests

  while (true) {
    // Receive request
    zmq::message_t request;
    socket.recv(request, zmq::recv_flags::none);
    std::string requestStr(static_cast<char*>(request.data()), request.size());
    std::cout << "Received request: " << requestStr << std::endl;

    // Simulate processing time
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Prepare response
    std::string responseStr = "Score: 100";
    zmq::message_t response(responseStr.size());
    memcpy(response.data(), responseStr.data(), responseStr.size());

    // Send response
    socket.send(response, zmq::send_flags::none);
  }
}