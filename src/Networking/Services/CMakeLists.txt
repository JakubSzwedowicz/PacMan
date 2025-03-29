set(SERVICES_SOURCES
        ServiceGameMessages.cpp
)

add_library(Services
        ${SERVICES_SOURCES}
)

target_include_directories(Services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Services PUBLIC Entities)

# Actually this should be controllers but leave it as is till I add them
target_link_libraries(Services PRIVATE DTOGameMessages)