#!/usr/bin/env bash
# Run the server in solo mode (no players) with ASCII rendering.
# --max_players 0 means no clients are expected; game starts immediately.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BINARY="$PROJECT_ROOT/build/debug/src/server/PacManServer"

if [[ "${1:-}" == "--build" ]] || [[ ! -f "$BINARY" ]]; then
    echo "Building PacManServer..."
    cmake --workflow --preset all-debug
    shift 2>/dev/null || true
fi

RENDER_INTERVAL="${RENDER_INTERVAL:-500}"

cd "$PROJECT_ROOT"
exec "$BINARY" \
    --maxPlayers 0 \
    --renderAscii \
    --renderInterval "$RENDER_INTERVAL" \
    --configPath "$PROJECT_ROOT/config/server.json"
