#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BINARY="$PROJECT_ROOT/build/debug/PacManServer"

if [[ "${1:-}" == "--build" ]] || [[ ! -f "$BINARY" ]]; then
    echo "Building PacManServer..."
    cmake --workflow --preset all-debug --fresh
    shift 2>/dev/null || true
fi

CONFIG="${PROJECT_ROOT}/config/server.json"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --config)
            CONFIG="$2"
            shift 2
            ;;
        *)
            shift
            ;;
    esac
done

cd "$PROJECT_ROOT"
exec "$BINARY" --config "$CONFIG"
