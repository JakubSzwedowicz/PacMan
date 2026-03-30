#!/usr/bin/env bash
# Run the server in solo mode (no players) with ASCII rendering.
# The test map has minPlayers=0, so the game starts immediately without
# waiting for any clients to connect.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BINARY="$PROJECT_ROOT/build/debug/src/server/PacManServer"

if [[ "${1:-}" == "--build" ]] || [[ ! -f "$BINARY" ]]; then
    echo "Building PacManServer..."
    cmake --workflow --preset all-debug
    shift 2>/dev/null || true
fi

PORT="${PORT:-7777}"
MAP_PATH="${MAP_PATH:-assets/maps/test.json}"
RENDER_INTERVAL="${RENDER_INTERVAL:-500}"

cd "$PROJECT_ROOT"
exec "$BINARY" \
    --port "$PORT" \
    --map_path "$MAP_PATH" \
    --max_players 0 \
    --render_ascii \
    --render_interval "$RENDER_INTERVAL"
