import numpy as np
from scipy.fft import fft
import os


def generate_fft_data_to_c_file(length, data_type="real"):
    if data_type == "real":
        input = np.random.randint(-32768, 32768, length)
        output = fft(input)[: len(input) // 2 + 1]

    elif data_type == "complex":
        input = np.random.randint(-32768, 32768, length) + 1j * np.random.randint(-32768, 32768, length)
        output = fft(input)
    else:
        raise ValueError("data_type must be 'real' or 'complex'")

    # Generate C array names
    input_array_name = f"fft_input_{length}_{data_type}"
    output_array_name = f"fft_output_{length}_{data_type}"

    # Generate file name
    file_name = f"./data/fft_data_{length}_{data_type}.c"

    with open(file_name, "w") as f:
        # Write input data as C array
        if data_type == "real":
            f.write(f"static q15_t {input_array_name}[] = {{\n    ")
            f.write(f"{input[1].astype(np.int16)}")
            for i in range(1, len(input)):
                f.write(f", {input[i].astype(np.int16)}")
            f.write("\n};\n\n")
        else:
            f.write(f"static q15_t {input_array_name}[] = {{\n")
            for value in input:
                f.write(f"    {value.real.astype(np.int16)}, {value.imag.astype(np.int16)},\n")
            f.write("};\n\n")

        # Write FFT result as C array
        f.write(f"static double {output_array_name}[] = {{\n    ")
        f.write(f"{output[0].real}, {output[0].imag}")
        for i in range(1, len(output)):
            f.write(f", {output[i].real}, {output[i].imag}")
        f.write("\n};\n\n")
        f.write("\n" f"#define FFT_LEN {length}\n" f"#define FFT_IN {input_array_name}\n" f"#define FFT_OUT {output_array_name}\n")

    print(f"Data written to {file_name}")


# Example usage
# generate_fft_data_to_c_file(8, "real")
generate_fft_data_to_c_file(64, "complex")
