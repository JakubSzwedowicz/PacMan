find_package(GTest CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

include(GoogleTest)

add_executable(TestPacManCore)
target_sources(TestPacManCore PRIVATE
    src/TestComponents.cpp
    src/TestMapsManager.cpp
    src/TestSimulation.cpp
)
target_link_libraries(TestPacManCore PRIVATE
    Core::Simulation
    GTest::gtest_main
    glaze::glaze
)
gtest_discover_tests(TestPacManCore)

add_executable(TestPacManClient)
target_sources(TestPacManClient PRIVATE src/TestScreenManager.cpp)
target_link_libraries(TestPacManClient PRIVATE
    Client::Screen
    GTest::gtest_main
)
gtest_discover_tests(TestPacManClient)
