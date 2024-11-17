#include "radar_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void append_to_file(const char *format, ...)
{
    char filename[] = "/media/huffer/SanDisk/projects/x86/LFMCW-Radar-DSP_C-Language/clog.txt";
    // 打开文件进行附加，如果文件不存在则创建新文件
    FILE *file = fopen(filename, "a");

    if (file == NULL) {
        // 如果打开文件失败，输出错误信息
        printf("Error opening file!\n");
        return;
    }

    // 声明 va_list，用于访问可变参数
    va_list args;
    va_start(args, format);

    // 使用 vfprintf 来格式化输出到文件
    vfprintf(file, format, args);

    // 结束可变参数的处理
    va_end(args);

    // 关闭文件
    fclose(file);
}


void radar_error(const char *reason, const char *file, int line, int error_code)
{
    printf("Radar error: %s, file %s, line %d\n", reason, file, line);
    append_to_file("Radar error: %s, file %s, line %d\n", reason, file, line);
    exit(114514);
}
