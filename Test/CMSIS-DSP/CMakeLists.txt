cmake_minimum_required(VERSION 3.10)
project(Test_CMSIS-DSP)

# 链接FFTW库
add_executable(test-cmsisdsp main.c)

target_sources(test-cmsisdsp PRIVATE
    test_arm_rfft_q15.c
)

# 添加依赖的目标
add_dependencies(test-cmsisdsp
    CMSISDSP
)

# 链接库
target_link_libraries(test-cmsisdsp PUBLIC
    CMSISDSP
)

# 添加头文件搜索路径
target_include_directories(test-cmsisdsp PUBLIC
    ${CMAKE_SOURCE_DIR}/ThirdParty/CMSIS-DSP/include
)

# 添加编译选项
target_compile_definitions(test-cmsisdsp PRIVATE
    -D__GNUC_PYTHON__ # CMSIS-DSP不适用CMSIS-CORE
)

add_test(test-cmsisdsp ${EXECUTABLE_OUTPUT_PATH}/test-cmsisdsp)

if(ENABLE_TEST_ARM_SQRT_Q31)
    add_executable(test_arm_sqrt_q31 test_arm_sqrt_q31.cc)
    target_link_libraries(test_arm_sqrt_q31 CMSISDSP GTest::gtest_main)
    gtest_discover_tests(test_arm_sqrt_q31)
endif()
