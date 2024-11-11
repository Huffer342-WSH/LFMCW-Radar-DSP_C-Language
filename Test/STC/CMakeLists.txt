

if(ENABLE_TEST_STC_CBITS)
    add_executable(test_stc_cbits test_stc_cbits.cc)
    target_compile_options(test_stc_cbits PRIVATE ${ADDRESSSANITIZER_FLAGS})
    target_link_libraries(test_stc_cbits stc asan GTest::gtest_main)
    gtest_discover_tests(test_stc_cbits)
endif()
