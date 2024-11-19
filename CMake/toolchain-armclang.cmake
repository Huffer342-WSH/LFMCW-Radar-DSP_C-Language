# -DCMAKE_TOOLCHAIN_FILE=CMake/toolchain-armclang.cmake

message("\nLoading ARM Compiler CMake file")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

set(CROSS_COMPILE ON) # 根目录中的 CMakeLists.txt 要设置 CROSS_COMPILE

# 强制使用指定的C/C++编译器
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID ARMCLANG)
set(CMAKE_CXX_COMPILER_ID ARMCLANG)

set(CPU_COMPILER_FLAGS "--target=arm-arm-none-eabi -mcpu=cortex-m0plus")
set(CPU_LINKER_FLAGS "--cpu Cortex-M0+")

# 查找工具链
set(TOOLCHAIN_PREFIX "")
set(TOOLCHAIN_PATH "E:/Program_Files/Keil/ARM/ARMCLANG") # 假如工具链不在环境变量中，可添加到CMAKE_PREFIX_PATH中是的CMake能找到
list(APPEND CMAKE_PREFIX_PATH "${TOOLCHAIN_PATH}/bin")

find_program(CUSTOM_C_COMPILER NAMES ${TOOLCHAIN_PREFIX}armclang)
find_program(CUSTOM_ASM_COMPILER NAMES ${TOOLCHAIN_PREFIX}armclang)
find_program(CUSTOM_CXX_COMPILER NAMES ${TOOLCHAIN_PREFIX}armclang)
find_program(CUSTOM_LINKER NAMES ${TOOLCHAIN_PREFIX}armlink)
find_program(CUSTOM_FROMELF NAMES ${TOOLCHAIN_PREFIX}fromelf)

message(CUSTOM_C_COMPILER: ${CUSTOM_C_COMPILER})

set(CMAKE_C_COMPILER ${CUSTOM_C_COMPILER})
set(CMAKE_ASM_COMPILER ${CUSTOM_ASM_COMPILER})
set(CMAKE_CXX_COMPILER ${CUSTOM_CXX_COMPILER})
set(CMAKE_LINKER ${CUSTOM_LINKER})
set(CMAKE_OBJCOPY ${CUSTOM_FROMELF})
set(CMAKE_FROMELF ${CUSTOM_FROMELF})

set(CMAKE_ASM_FLAGS "${CPU_COMPILER_FLAGS} -masm=auto -c -Wa,armasm,--diag_suppress=A1950W -c")

# set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -xc -std=c99")
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} ${CPU_COMPILER_FLAGS} -c")
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -fno-rtti  -funsigned-char -fshort-enums -fshort-wchar")
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -gdwarf-4  -ffunction-sections")
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -Wno-packed -Wno-missing-variable-declarations -Wno-missing-prototypes -Wno-missing-noreturn -Wno-sign-conversion -Wno-nonportable-include-path -Wno-reserved-id-macro -Wno-unused-macros -Wno-documentation-unknown-command -Wno-documentation -Wno-license-management -Wno-parentheses-equality -Wno-reserved-identifier")

set(CMAKE_CXX_FLAGS " ${CMAKE_C_FLAGS}")

add_link_options(
    ${CPU_LINKER_FLAGS} --diag_suppress 6314,6329 --summary_stderr --load_addr_map_info --xref --callgraph --symbols --info=summarysizes,sizes
)

# 添加编译指令
add_compile_options(
    "$<$<CONFIG:Debug>:-O0;-g3;-DDEBUG>"
    "$<$<CONFIG:Release>:-Ofast;-DNDEBUG>"
    "$<$<CONFIG:MinSizeRel>:-Os;-DNDEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:-O2;-g;-DDEBUG>"
)
