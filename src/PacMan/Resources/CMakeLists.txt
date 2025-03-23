# Define the source and destination directories
set(RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/src")

# Copy the Resources directory to the binary directory
file(COPY ${RESOURCE_PATH} DESTINATION ${DESTINATION_PATH})
#add_library(Resources
#        ResourcesHandler.cpp)
#
#target_compile_features(Resources PRIVATE cxx_std_17)
#
#target_compile_definitions(Resources PRIVATE RESOURCE_PATH="${RESOURCE_PATH}")
