find_package(Catch2 3 REQUIRED)
include(Catch)

add_executable(mockup_tests read_graph.test.cpp)
target_link_libraries(mockup_tests PRIVATE mockup Catch2::Catch2WithMain)

catch_discover_tests(mockup_tests)
