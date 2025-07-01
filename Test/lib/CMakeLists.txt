
if(ENABLE_TEST_BITSET)
    add_executable(test_bitset test_bitset.cc)
    target_link_libraries(test_bitset RadarDP_Fixed GTest::gtest_main)
    gtest_discover_tests(test_bitset)
endif()

if(ENABLE_TEST_MM)
    add_executable(test_mm test_mm.cc)
    target_link_libraries(test_mm RadarDP_Fixed GTest::gtest_main)
    gtest_discover_tests(test_mm)
endif()
