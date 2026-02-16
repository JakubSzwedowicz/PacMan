# --- Find all dependencies once at this level ---

# Core dependencies
find_package(EnTT CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Utils CONFIG REQUIRED)

# Network (used by both client and server)
find_package(unofficial-enet CONFIG REQUIRED)

# Client-only dependencies
find_package(SFML COMPONENTS Graphics Window System Audio Network CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

# --- Subdirectories ---
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
