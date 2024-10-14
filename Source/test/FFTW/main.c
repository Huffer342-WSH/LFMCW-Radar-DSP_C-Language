#include <stdio.h>
#include <math.h>
#include <string.h>

#include <fftw3.h>

/* DFT测试数据 */
#include "data/fft_data_8_real.c" // 务必放在#include <fftw3.h>后面

int main()
{
	printf("FFTW Test\n");
	int N = 8; // FFT点数

	// 1. 分配输出数组,输入和输出参考由数据文件提供
	fftw_complex out[FFT_LEN / 2 + 1]; // 复数输出数组 (因为是实数FFT，输出是对称的，只保留一半)

	// 2. 创建FFTW计划 (plan)
	fftw_plan plan = fftw_plan_dft_r2c_1d(N, FFT_IN, out, FFTW_ESTIMATE);

	// 3. 执行FFT变换
	fftw_execute_dft_r2c(plan, FFT_IN, out);

	// 5. 打印结果
	printf("| i   |       results           |         reference       |\n");
	for (int i = 0; i < N / 2 + 1; i++) {
		printf("| %-4d| %+.3e  %+.3ei | %+.3e  %+.3ei |\n", i, out[i][0], out[i][1],
		       FFT_OUT[i][0], FFT_OUT[i][1]);
	}
	puts("\n");

	double mse = 0;
	for (int i = 0; i < N / 2 + 1; i++) {
		mse += (out[i][0] - FFT_OUT[i][0]) * (out[i][0] - FFT_OUT[i][0]) +
		       (out[i][1] - FFT_OUT[i][1]) * (out[i][1] - FFT_OUT[i][1]);
	}
	mse /= (N / 2 + 1);
	printf("MSE = %.3e\n", mse);

	// 6. 清理和释放资源
	fftw_destroy_plan(plan);
	fftw_cleanup();

	return 0;
}
