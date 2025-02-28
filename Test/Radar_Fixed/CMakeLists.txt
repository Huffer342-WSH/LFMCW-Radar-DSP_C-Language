set(public_libraries lfmcwradar_fixed GTest::gtest_main ${ADDRESSSANITIZER_LINKER_FLAGS})



if(ENABLE_TEST_RADAR_CFAR)
    add_executable(test_radar_cfar test_radar_cfar.cc)
    target_link_libraries(test_radar_cfar ${public_libraries})
    gtest_discover_tests(test_radar_cfar)
endif()

if(ENABLE_TEST_RADAR_CLAC_MAGSPEC2D)
    add_executable(test_radar_clac_magSpec2D test_radar_clac_magSpec2D.cc)
    target_link_libraries(test_radar_clac_magSpec2D ${public_libraries})
    gtest_discover_tests(test_radar_clac_magSpec2D)
endif()

if(ENABLE_TEST_RADAR_CLUSTER)
    add_executable(test_radar_cluster test_radar_cluster.cc)
    target_link_libraries(test_radar_cluster ${public_libraries})
    gtest_discover_tests(test_radar_cluster)
endif()

if(ENABLE_TEST_RADAR_MATH)
    add_executable(test_radar_math test_radar_math.cc)
    target_link_libraries(test_radar_math ${public_libraries})
    gtest_discover_tests(test_radar_math)
endif()

if(ENABLE_TEST_RADAR_MATRIX)
    add_executable(test_radar_matrix test_radar_matrix.cc)
    target_link_libraries(test_radar_matrix ${public_libraries})
    gtest_discover_tests(test_radar_matrix)
endif()

if(ENABLE_TEST_TRACK_KALMAN)
    add_executable(test_track_kalman test_track_kalman.cc)
    target_link_libraries(test_track_kalman ${public_libraries})
    gtest_discover_tests(test_track_kalman)
endif()

if(ENABLE_TEST_TRACK_TARGETS)
    add_executable(test_track_targets test_track_targets.cc)
    target_link_libraries(test_track_targets ${public_libraries})
    gtest_discover_tests(test_track_targets)
endif()

if(ENABLE_TEST_TRACK_ASSOCIATOR)
    add_executable(test_track_associator test_track_associator.cc)
    target_link_libraries(test_track_associator ${public_libraries})
    gtest_discover_tests(test_track_associator)
endif()

if(ENABLE_TEST_TRACK_INITIATOR)
    add_executable(test_track_initiator test_track_initiator.cc)
    target_link_libraries(test_track_initiator ${public_libraries})
    gtest_discover_tests(test_track_initiator)
endif()

if(ENABLE_TEST_TRACK_DELETER)
    add_executable(test_track_deleter test_track_deleter.cc)
    target_link_libraries(test_track_deleter ${public_libraries})
    gtest_discover_tests(test_track_deleter)
endif()
