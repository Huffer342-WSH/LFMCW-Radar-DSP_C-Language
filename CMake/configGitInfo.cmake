# 获取当前 Git 提交的哈希值
function(get_git_hash git_hash)
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE git_result
        OUTPUT_VARIABLE git_hash_temp
        ERROR_VARIABLE git_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT git_result EQUAL 0)
        message(FATAL_ERROR "Failed to get Git hash: ${git_error}")
    else()
        set(${git_hash} ${git_hash_temp} PARENT_SCOPE)
    endif()
endfunction()

# 获取当前 Git 提交的时间
function(get_git_commit_time git_commit_time)
    execute_process(
        COMMAND git log -1 --format=%cd
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE git_result_time
        OUTPUT_VARIABLE git_commit_time_temp
        ERROR_VARIABLE git_error_time
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT git_result_time EQUAL 0)
        message(FATAL_ERROR "Failed to get Git commit time: ${git_error_time}")
    else()
        set(${git_commit_time} ${git_commit_time_temp} PARENT_SCOPE)
    endif()
endfunction()

# 调用函数
get_git_hash(GIT_HASH)
get_git_commit_time(GIT_COMMIT_TIME)

# 输出获取到的哈希值和提交时间
message(STATUS "Current Git hash: ${GIT_HASH}")
message(STATUS "Git commit time: ${GIT_COMMIT_TIME}")
