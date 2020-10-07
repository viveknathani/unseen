#!/usr/bin/env python3

one = open('key.txt', 'r')
two = open('../algorithm/final.txt', 'r')

one_ls = []
two_ls = []
for each in one:
    one_ls.append(each)
for each in two:
    two_ls.append(each)

c = 0
for i in range(0, 61, 1):
    if one_ls[i] == two_ls[i]:
        c += 1
    else :
        print(one_ls[i])
        print(two_ls[i])    

print(c)
