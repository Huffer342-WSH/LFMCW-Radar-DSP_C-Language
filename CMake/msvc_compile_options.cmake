if(MSVC)
    # 关闭警告 C4819: 无效的字符。避免中文注释被警告
    add_compile_options(
        "/wd4819"
    )

    # 关闭 /showIncludes
    string(REPLACE "/showIncludes" "" CMAKE_DEPFILE_FLAGS_C "${CMAKE_DEPFILE_FLAGS_C}")
    string(REPLACE "/showIncludes" "" CMAKE_DEPFILE_FLAGS_CXX "${CMAKE_DEPFILE_FLAGS_CXX}")
endif()
