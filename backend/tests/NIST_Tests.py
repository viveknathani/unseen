#!/usr/bin/env python3
import subprocess

keys = []
IVs = []
inputs = []
outputs = []

file = open('NIST_Tests.txt', 'r')

shift = False

for line in file:
    if(line.startswith('[DECRYPT]')):
        shift = True

    if(line.startswith('KEY')):
        temp = line.replace('\n', '')
        temp = temp.replace('KEY = ', '')
        keys.append(temp)

    if(line.startswith('IV')):
        temp = line.replace('\n', '')
        temp = temp.replace('IV = ', '')
        IVs.append(temp)

    if(line.startswith('PLAINTEXT')):
        temp = line.replace('\n', '')
        temp = temp.replace('PLAINTEXT = ', '')
        if(shift):
            outputs.append(temp)
        else:    
            inputs.append(temp)

    if(line.startswith('CIPHERTEXT')):
        temp = line.replace('\n', '')
        temp = temp.replace('CIPHERTEXT = ', '')
        if(shift):
            inputs.append(temp)
        else:
            outputs.append(temp)     

count = 0

for i in range(0, 256, 1):
    task = str('0')
    if(i >= 128):
        task = str('1') 

    runthis = '../bin/./algo_test.o ' +  str(inputs[i]) + ' ' + str(keys[i]) + ' ' + str(IVs[i]) + ' ' + task
    result = subprocess.run(runthis, stdout=subprocess.PIPE, shell=True)
    if(result.stdout.decode('utf-8') == outputs[i] + '\n'):
        count += 1

total = 256
print("Tests passed :", count, "/", total)
