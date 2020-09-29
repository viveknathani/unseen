#!/usr/bin/env python3

import subprocess
import os 

# before running this, make sure that a data folder exists in backend
# images inside it should be numbered in a linear fashion starting from 1

path_to_bin = "../bin/"
path_to_data = "../data/"
run_valgrind = "valgrind --quiet "

delete_prev_outputs = "cd " + path_to_data + " && find . -type f -name out_\\* -exec rm {} \\; && cd ../tests"
subprocess.run(delete_prev_outputs, shell=True)

count_jpg = 0
count_png = 0
for f in os.listdir(path_to_data):
    if(os.path.splitext(f)[1].lower() in ('.jpg')):
        count_jpg += 1
    if(os.path.splitext(f)[1].lower() in ('.png')):
        count_png += 1    

combined_str = run_valgrind + path_to_bin + "./lib_test.o " + path_to_data

print("Running tests on jpg files...")
for i in range(1, count_jpg + 1, 1):
    print("Log->lib_memory.py Test",i)
    run_str = combined_str + str(i) + ".jpg " + path_to_data +"out_" + str(i) + ".jpg"  + " 0"
    subprocess.run(run_str, shell=True)

print("Running tests on png files...")   
for i in range(1, count_png + 1, 1):
    print("Log->lib_memory.py Test",i)
    run_str = combined_str + str(i) + ".png " + path_to_data +"out_" + str(i) + ".png" + " 1"
    subprocess.run(run_str, shell=True)
