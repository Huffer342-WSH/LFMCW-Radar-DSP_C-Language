message(STATUS "Configuring AddressSanitizer")

if(CMAKE_SYSTEM_NAME MATCHES "Windows" AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # 如果在 Windows 上使用 MinGW 编译器，跳过配置
    message(STATUS "Skipping AddressSanitizer configuration for MinGW")
    return()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # 对于 Clang 或 GCC，启用 AddressSanitizer，并生成调试信息
    message(STATUS "Configuring AddressSanitizer for Clang/GCC")
    set(ADDRESSSANITIZER_FLAGS "-fsanitize=address")
    set(ADDRESSSANITIZER_LINKER_FLAGS "-lasan")
    set(ADDRESSSANITIZER_LIBRARIES "asan")

elseif(MSVC)
    # 对于 MSVC，启用 AddressSanitizer
    message(STATUS "Configuring AddressSanitizer for MSVC")
    set(ADDRESSSANITIZER_FLAGS "/fsanitize=address")
    # MSVC 不需要设置 -lasan 链接选项

else()
    message(FATAL_ERROR "Unknown compiler.")
endif()

# 将 AddressSanitizer 的编译和链接选项应用到目标
message(STATUS "AddressSanitizer compile flags: ${ADDRESSSANITIZER_FLAGS}")
message(STATUS "AddressSanitizer linker flags: ${ADDRESSSANITIZER_LINKER_FLAGS}")
