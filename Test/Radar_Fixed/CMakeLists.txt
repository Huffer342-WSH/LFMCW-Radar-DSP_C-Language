

if(ENABLE_TEST_RADAR_CLAC_MAGSPEC2D)
    add_executable(test_radar_clac_magSpec2D test_radar_clac_magSpec2D.cc)
    target_link_libraries(test_radar_clac_magSpec2D lfmcwradar_fixed GTest::gtest_main)
    gtest_discover_tests(test_radar_clac_magSpec2D)
endif()
