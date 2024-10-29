import subprocess
import sys
import os
import json
from datetime import datetime


sort_key = lambda d: dict(sorted({int(k): v for k, v in d.items()}.items()))
expressions = [
    "1.0 + 0.5 * x - 0.125 * x * x + 0.0625 * x * x * x - 0.0390625 * x * x * x * x",
    "1.0 / (sqrt(x + 1.0) + sqrt(x))",
    "(exp(x) - 1.0) / x",
    "(exp(x) - 1.0) / log(exp(x))",
    "exp(x) - 1.0",
    "log(x + 1.0) - log(x)",
    "1.0 / x - 1.0 / tan(x)",
    "(1.0 - cos(x)) / (x * x)",
    "exp(x) / (exp(x) - 1.0)",
    "(x - sin(x)) / (x - tan(x))",
    "exp(x) - 2.0 + exp(-x)",
    "(4.0 * x) / (x / 1.11 + 1.0)",
    "(4.0 * x * x) / (1 + x / 1.11 + x / 1.11)",
    "log(exp(x) - 1.0)",
    "x - (x * x * x) / 6.0 + (x * x * x * x * x) / 120.0 - (x * x * x * x * x * x * x) / 5040.0",
    "((35000000.0 + ((0.401 * (1000.0 / x)) * (1000.0 / x))) * (x - (1000.0 * 4.27e-5))) - ((1.3806503e-23 * 1000.0) * 300.0)",
    "(1.0 - cos(x)) / (x * x)",
    "(((x + 1.0) * log(x + 1.0)) - (x * log(x))) - 1.0",
    "cbrt(x + 1) - cbrt(x)",
    "((1.0 / (x + 1.0)) - (2.0 / x)) + (1.0 / (x - 1.0))",
    "1.0/(x+1) - (1.0/x)",
    "1.0/sqrt(x) - 1.0 / sqrt(x+1)",
    "1.0 / tan(x + 1) - 1.0 / tan(x)",
    "(1.0 - cos(x)) / sin(x)",
    "sqrt(x + 1) - sqrt(x)",
    "(x - 1) / (x * x - 1)",
    "1.0 / (x + 1.0)",
    "x / ( x + 1)",
    "(0.954929658551372 * x) - (0.12900613773279798 * ((x * x) * x))",
    "(-x * x * x) / 6.0",
    "log(1 - x) / log(1 + x)",
    "log((1 - x) / (1 + x))",
]


def get_result_one_interval(expr, start, end, fp):
    # Compile the main program
    # subprocess.run(["make"], check=True)
    # subprocess.run(["bin/errordetect.exe", expr], check=True)

    print("-------MPFR high-precision version code generation-------")

    # Change directory and compile the detectModule
    os.chdir("detectModule")
    subprocess.run(["make"], check=True)
    subprocess.run(["bin/errorDetect.exe", str(start), str(end)], check=True, stdout=fp)

    # Change back to the original directory
    os.chdir("..")


def get_result_one_expression(i, expr, interval_file, fp):
    # Run the first command and capture stderr if an error occurs
    subprocess.run(["bin/errordetect.exe", expr], check=True, stderr=subprocess.PIPE)

    # Change directory and compile the detectModule
    os.chdir("detectModule")

    try:
        subprocess.run(["make"], check=True, stderr=subprocess.PIPE)

        # Run the second command and capture stderr if an error occurs
        subprocess.run(
            ["bin/errorDetect.exe", str(i), interval_file],
            check=True,
            stdout=fp,
            stderr=subprocess.PIPE,
        )

    except subprocess.CalledProcessError as e:
        # Print the error message if the command fails
        print(f"Error occurred: {e.stderr.decode()}")

    finally:
        # Change back to the original directory
        os.chdir("..")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: python {sys.argv[0]} <range_file>")
        sys.exit(1)

    
    with open(sys.argv[1], "r") as f:
        json_data = sort_key(json.load(f))
    
    # !=============================================================
    with open("/ICE/data/hsed/data_origin.json", "r") as f:
        orig_data = sort_key(json.load(f))

    with open("result.txt", "w") as fp:
        for i, func_data in json_data.items():
            if not func_data["ranges"]:
                continue

            # ! Debug
            if i not in [2]:
                continue

            fp.write(f"Time: {datetime.now().strftime("%a %b %d %H:%M:%S %Y")}\n\n")
            fp.write(f"Function Index: {i}\n")
            fp.flush()
            get_result_one_expression(i, expressions[i], sys.argv[1], fp)

    with open("result.txt", "r") as f:
        result_text = f.read()

    import re

    pattern = r"Function Index: (\d+)\n(?:.*\n)*?float-precision layer: x = ([\d.e-]+), maximumULP = ([\d.e-]+)(?:.*\n)*?double-precision layer: x = ([\d.e-]+), maximumULP = ([\d.e-]+), maximumRelative = ([\d.eE+-]+)(?:.*\n)*?Layer1 Time: ([\d.e-]+)\nLayer2 Time: ([\d.e-]+)\nLayer3 Time: ([\d.e-]+)"
    # pattern = r"Function Index: (\d+)\n(?:.*\n)*?float-precision layer: x = ([\d.e-]+), maximumULP = ([\d.e-]+)\n\ndouble-precision layer: x = ([\d.e-]+), maximumULP = ([\d.e-]+).*\nElapsed Time: ([\d.e-]+)"
    
    result = {
        int(match[0]): {
            "x_2": float(match[1]),
            "ulp_2": float(match[2]),
            "x": float(match[3]),
            "ulp": float(match[4]),
            "rel_err": float(match[5]),
            "time_1": float(match[6]),
            "time_2": float(match[7]),
            "time_3": float(match[8]),
        }
        for match in re.findall(pattern, result_text)
    }

    print(result)
    # !=============================================================
    # with open(sys.argv[1], "r") as f:
    #     inputs_data = sort_key(json.load(f))
    # with open("result.txt", "w") as fp:
    #     for i, func_input in inputs_data.items():
    #         get_result_one_expression(i, expressions[i], sys.argv[1], fp)
        
