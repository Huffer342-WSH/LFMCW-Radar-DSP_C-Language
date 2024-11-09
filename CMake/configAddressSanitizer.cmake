# configAddressSanitizer.cmake

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # 对于 Clang 或 GCC，启用 AddressSanitizer，并生成调试信息
    message(STATUS "Configuring AddressSanitizer for Clang/GCC")
    set(ADDRESSSANITIZER_FLAGS "-fsanitize=address")

elseif(MSVC)
    # 对于 MSVC，启用 AddressSanitizer
    message(STATUS "Configuring AddressSanitizer for MSVC")
    set(ADDRESSSANITIZER_FLAGS "/fsanitize=address")
else()
    message(WARNING "AddressSanitizer not supported for this compiler.")
    return()
endif()
