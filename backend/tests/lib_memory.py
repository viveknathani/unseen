#!/usr/bin/env python3

import subprocess

# list of commands

path_to_bin = "../bin/"
path_to_data = "../data/"
run_valgrind = "valgrind --quiet "
separator = "; "


combined_str = run_valgrind + path_to_bin + "./lib_test " + path_to_data

for i in range(1, 208, 1):
    print("Log->lib_memory.py Test",i)
    run_str = combined_str + str(i) + ".jpg " + path_to_data +"out_" + str(i) + ".jpg" 
    subprocess.run(run_str, shell=True)
