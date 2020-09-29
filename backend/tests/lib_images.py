from PIL import Image
import imagehash
import subprocess
import os

path_to_data = "../data/"

count_jpg = 0
count_png = 0
for f in os.listdir(path_to_data):
    if(os.path.splitext(f)[1].lower() in ('.jpg')):
        count_jpg += 1
    if(os.path.splitext(f)[1].lower() in ('.png')):
        count_png += 1   

count_jpg = int(count_jpg/2)
count_png = int(count_png/2)

hash_cutoff = 1

print("Running tests on jpg files...")
for i in range(1, count_jpg + 1, 1):
    print("Log->lib_images.py Test",i)
    file1 = path_to_data + str(i) + ".jpg"
    file2 = path_to_data +"out_" + str(i) + ".jpg"
    hash1 = imagehash.average_hash(Image.open(file1)) 
    hash2 = imagehash.average_hash(Image.open(file2)) 
    if(hash1 - hash2 < hash_cutoff) :
         print("OK. Test passed.")
    else :
        print("Do not seem visually similar.")    

    

print("Running tests on png files...")   
for i in range(1, count_png + 1, 1):
    print("Log->lib_images.py Test",i)
    file1 = path_to_data + str(i) + ".png"
    file2 = path_to_data +"out_" + str(i) + ".png"
    hash1 = imagehash.average_hash(Image.open(file1)) 
    hash2 = imagehash.average_hash(Image.open(file2)) 
    if(hash1 - hash2 < hash_cutoff) :
        print("OK. Test passed.")
    else :
        print("Do not seem visually similar.") 
