set(CONTROLLERS_SOURCES
        RequestResponseHandler.cpp
)

add_library(Controllers ${CONTROLLERS_SOURCES})

target_compile_features(Controllers PUBLIC cxx_std_17)
target_include_directories(Controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Controllers PUBLIC Utils GameMessages GameLogic)
