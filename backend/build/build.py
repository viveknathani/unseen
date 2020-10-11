#!/usr/bin/env python3

import subprocess
import importlib.util
import os
import json

# dependencies 
file = open("depend.json")
data = json.load(file)
file.close()

for apt_name in data["ubuntu_apt"]:
    cmd = "dpkg-query -W -f='${Status}' " + str(apt_name) +  " 2>/dev/null | grep -c \"ok installed\" "
    print("Checking for package", apt_name)
    result = subprocess.run(cmd, stdout=subprocess.PIPE, shell=True)
    if(result.stdout == b'1\n'):
        print('OK')
    else :
        install_package = "sudo apt install " + str(apt_name)
        subprocess.run(install_package, shell=True)    

for py_pack in data["python_packages"]:
    import_name = list(py_pack.keys())[0]
    install_name = list(py_pack.values())[0]
    result = importlib.util.find_spec(import_name)
    print("Checking for package", import_name)
    if result is None:
        print("Doesn't exist, installing...")
        install_package = "pip3 install " + install_name
        subprocess.run(install_package, shell=True)
    else:
        print("OK")   

# prepare lib_test.o
if(os.path.isdir("../bin") is False):
    subprocess.run("mkdir ../bin", shell=True)
lib_compile = "g++ run.cpp ../lib/Image.cpp -ljpeg -lpng -o ../bin/lib_test.o"
print("Compiling for lib_test.o...")
subprocess.run(lib_compile, shell=True)

# prepare algo_test.o
algo_compile = "g++ algo.cpp ../algorithm/AES256.cpp -o ../bin/algo_test.o"
print("Compiling for algo_test.o...")
subprocess.run(algo_compile, shell=True)

# prepare combine_test.o
combine_compile = "g++ combine.cpp ../algorithm/AES256.cpp ../lib/Image.cpp -ljpeg -lpng -o ../bin/combine_test.o"
print("Compiling for combine_test.o")
subprocess.run(combine_compile, shell=True)

print("Build Successful!")