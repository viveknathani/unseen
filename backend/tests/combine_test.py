import subprocess
import os

path_to_data = "../data/"

count_jpg = 0
count_png = 0
for files in os.listdir(path_to_data):
    if(os.path.splitext(files)[1].lower() in ('.jpg')):
        count_jpg += 1
    if(os.path.splitext(files)[1].lower() in ('.png')):
        count_png += 1

hex_key = ''
for i in range(0, 64, 1):
    hex_key += str('0')

def produce_encrypted(image_number, image_type):        

    input_file_name = str(image_number) + str('.') + str(image_type)
    input_file_path = path_to_data + input_file_name

    output_file_name = str('E')
    if(image_type == 'jpg') :
        output_file_name += (str('J') + str(image_number))
    if(image_type == 'png') :
        output_file_name += (str('P') + str(image_number))    

    output_file_name += '.png'    
    output_file_path = path_to_data + output_file_name 

    run_this = '../bin/./combine_test.o ' + input_file_path + ' ' + output_file_path + ' ' + hex_key + ' 0'
    return run_this

def produce_decrypted(image_number, image_type):  

    input_file_name = str('E')
    if(image_type == 'jpg') :
        input_file_name += (str('J') + str(image_number))
    if(image_type == 'png') :
        input_file_name += (str('P') + str(image_number))
    input_file_name += '.png'    
    input_file_path = path_to_data + input_file_name     

    output_file_name = str('D')
    if(image_type == 'jpg') :
        output_file_name += (str('J') + str(image_number))
    if(image_type == 'png') :
        output_file_name += (str('P') + str(image_number))
    output_file_name += '.png'    
    output_file_path = path_to_data + output_file_name

    binary_file_path = path_to_data
    if(image_type == 'jpg'):
        binary_file_path += ('EJ' + str(image_number) + '.dat')
    if(image_type == 'png'):
        binary_file_path += ('EP' + str(image_number) + '.dat')    
    run_this = '../bin/./combine_test.o ' + input_file_path + ' ' + output_file_path + ' ' + hex_key + ' 1 ' + binary_file_path
    print(run_this)
    return run_this

run_this_only = ''

for i in range(1, count_jpg + 1, 1):
    run_this_only += produce_encrypted(i, 'jpg')
    run_this_only += str(';')
    run_this_only += produce_decrypted(i, 'jpg')
    run_this_only += str(';')

for i in range(1, count_png + 1, 1):
    run_this_only += produce_encrypted(i, 'png')
    run_this_only += str(';')
    run_this_only += produce_decrypted(i, 'png')
    run_this_only += str(';')    

subprocess.run(run_this_only, shell=True)


