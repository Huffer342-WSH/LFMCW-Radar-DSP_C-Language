cmake_minimum_required(VERSION 3.10)
project(Test_CMSIS-DSP)

if(ENABLE_TEST_ARM_CFFT_Q15)
    add_executable(test_arm_cfft_q15 test_arm_cfft_q15.cc)
    target_link_libraries(test_arm_cfft_q15 CMSISDSP GTest::gtest_main)
    gtest_discover_tests(test_arm_cfft_q15)
endif()

if(ENABLE_TEST_ARM_SQRT_Q31)
    add_executable(test_arm_sqrt_q31 test_arm_sqrt_q31.cc)
    target_link_libraries(test_arm_sqrt_q31 CMSISDSP GTest::gtest_main)
    gtest_discover_tests(test_arm_sqrt_q31)
endif()
