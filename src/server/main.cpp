#include "server/app/ServerApp.hpp"

int main(int argc, char *argv[]) {
  pacman::server::app::ServerApp app;
  return app.main(argc, argv);
}
