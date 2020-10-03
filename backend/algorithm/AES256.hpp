#ifndef AES256_H
#define AES256_H

#include <string>

#define ENCRYPT 0
#define DECRYPT 1

class AES256
{
    private :
        std::string hexText;
        unsigned char state[16];

    public :
        AES256(std::string hexText, std::string key, int task);    
        void mixColumns(unsigned char state[], bool inverse);
        void shiftRows(unsigned char state[], bool inverse);
};

#endif