# PacMan project

## Building The project
(optional) First install dependencies
```
./scripts/setup_workspace.sh
```

(required) Then build the project
```
cmake --workflow --preset all-debug
```

## Running
To run the game both the Server and a Client must be running. Client is just a relatively thin wrapper on the data server broadcasts with some entities movement prediction. Client spawns a Server automatically when hosting a game but a server can be also started manually with optional ascii rendering. So there are two options to start a game:

1. Client + standalone server
* Start standalone server in CLI: `./scripts/run_server_ascii.sh` And find the port number it opened in the logs *serverLog.txt*
* Then start a client with `./scripts/run_client.sh`, insert the port number and join the game

2. Client + spawned server
* Start the client with `./scripts/run_client.sh`, host the game, find the log file *clientLog.txt*, get the port number, start another client, insert the port number and join the game.

