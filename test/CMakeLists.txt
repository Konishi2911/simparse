add_executable(simparse_tests parse_test.cc)
target_include_directories(simparse_tests PRIVATE ${PROJECT_BINARY_DIR})
target_link_libraries(simparse_tests GTest::gtest GTest::gtest_main ${OpenMP_CXX_LIBRARIES})
gtest_discover_tests(simparse_tests)