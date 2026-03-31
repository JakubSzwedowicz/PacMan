#!/usr/bin/env bash
# Start a multiplayer game: server (with ASCII rendering) + one or more clients.
# The server ASCII output streams to this terminal; client windows open separately.
#
# Usage:
#   ./run_game_ascii.sh [--build] [--players N]
#
# Options:
#   --build      Rebuild all targets before running (default: skip if binaries exist).
#   --players N  Number of client windows to open (default: 1).
#
# Environment overrides:
#   PORT             Server port           (default: 7777)
#   SERVER_ADDRESS   Address clients use   (default: 127.0.0.1)
#   MAP_PATH         Map file path         (default: assets/maps/test.json)
#   RENDER_INTERVAL  ASCII render interval (default: 500 ms)
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SERVER_BIN="$PROJECT_ROOT/build/debug/PacManServer"
CLIENT_BIN="$PROJECT_ROOT/build/debug/PacManClient"

# ---- argument parsing -------------------------------------------------------
NUM_PLAYERS=1
DO_BUILD=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --build)    DO_BUILD=true; shift ;;
        --players)  NUM_PLAYERS="$2"; shift 2 ;;
        *)          shift ;;
    esac
done

# ---- build if requested or binary missing -----------------------------------
if $DO_BUILD || [[ ! -f "$SERVER_BIN" ]] || [[ ! -f "$CLIENT_BIN" ]]; then
    echo "Building all targets..."
    cmake --workflow --preset all-debug --fresh
fi

# ---- config -----------------------------------------------------------------
PORT="${PORT:-7777}"
SERVER_ADDRESS="${SERVER_ADDRESS:-127.0.0.1}"
MAP_PATH="${MAP_PATH:-assets/maps/test.json}"
RENDER_INTERVAL="${RENDER_INTERVAL:-500}"

cd "$PROJECT_ROOT"

# ---- start server in background ---------------------------------------------
echo "Starting server on port $PORT (ASCII rendering every ${RENDER_INTERVAL}ms)..."
"$SERVER_BIN" \
    --port "$PORT" \
    --mapPath "$MAP_PATH" \
    --maxPlayers "$NUM_PLAYERS" \
    --renderAscii \
    --renderInterval "$RENDER_INTERVAL" &
SERVER_PID=$!

# Kill server when this script exits for any reason.
trap 'echo "Stopping server (pid $SERVER_PID)..."; kill "$SERVER_PID" 2>/dev/null; wait "$SERVER_PID" 2>/dev/null || true' EXIT

# Give the server a moment to bind its port.
sleep 0.3

# ---- launch clients ---------------------------------------------------------
CLIENT_PIDS=()
for ((i = 1; i <= NUM_PLAYERS; i++)); do
    echo "Launching client $i..."
    "$CLIENT_BIN" \
        --serverAddress "$SERVER_ADDRESS" \
        --serverPort "$PORT" \
        --mapPath "$MAP_PATH" &
    CLIENT_PIDS+=($!)
done

# ---- wait for all clients to exit -------------------------------------------
for pid in "${CLIENT_PIDS[@]}"; do
    wait "$pid" 2>/dev/null || true
done

echo "All clients exited."
