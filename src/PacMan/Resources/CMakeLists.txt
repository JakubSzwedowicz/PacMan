# Define the source and destination directories
set(RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(RESOURCES_DESTINATION_PATH "${CMAKE_BINARY_DIR}/Resources" CACHE PATH "Path to the resources directory")
file(MAKE_DIRECTORY ${RESOURCES_DESTINATION_PATH})

# Copy the Resources directory to the binary directory
configure_file("${RESOURCE_PATH}/Board1.txt" ${RESOURCES_DESTINATION_PATH} COPYONLY)
#add_library(Resources
#        ResourcesHandler.cpp)
#
#target_compile_features(Resources PRIVATE cxx_std_17)
#
#target_compile_definitions(Resources PRIVATE RESOURCE_PATH="${RESOURCE_PATH}")
