add_executable(PacManServer
    main.cpp
)

target_link_libraries(PacManServer
    PRIVATE
        PacManCore
        unofficial::enet::enet
)
