#include "AES256.hpp"
#include <iostream>

void mixColumns(unsigned char state[], bool inverse)
{

}

void shiftRows(unsigned char state[], bool inverse)
{
    unsigned char x;
    if(inverse)                 //for decryption
    {
        x = state[1];
        state[1] = state[13];
        state[13] = state[9];
        state[9] = state[5];
        state[5] = x;
        x = state[2];
        state[2] = state[10];
        state[10] = x;
        x = state[3];
        state[3] = state[7];
        state[7] = state[11];
        state[11] = state[15];
        state[15] = x;
        x = state[6];
        state[6] = state[14];
        state[14] = x;
    }

    else                        //for encryption
    {
        x = state[1];
        state[1] = state[5];
        state[5] = state[9];
        state[9] = state[13];
        state[13] = x;
        x = state[2];
        state[2] = state[10];
        state[10] = x;
        x = state[3];
        state[3] = state[15];
        state[15] = state[11];
        state[11] = state[7];
        state[7] = x;
        x = state[6];
        state[6] = state[14];
        state[14] = x;
    }
}

