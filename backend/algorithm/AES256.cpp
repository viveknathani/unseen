#include "AES256.hpp"
#include "AESLookups.hpp"
#include <iostream>

void AES256::mixColumns(unsigned char state[], bool inverse)
{
    unsigned char temp[4];
    if(inverse)                 //for decryption
    {
        for(int i=0;i<4;i++)
        {
            temp[0] = (LOOKUP_MUL_14[state[(4*i)+0]]) ^ (LOOKUP_MUL_11[state[(4*i)+1]]) 
                ^ (LOOKUP_MUL_13[state[(4*i)+2]]) ^ (LOOKUP_MUL_9[state[(4*i)+3]]);

            temp[1] = (LOOKUP_MUL_9[state[(4*i)+0]]) ^ (LOOKUP_MUL_14[state[(4*i)+1]])
                ^ (LOOKUP_MUL_11[state[(4*i)+2]]) ^ (LOOKUP_MUL_13[state[(4*i)+3]]);

            temp[2] = (LOOKUP_MUL_13[state[(4*i)+0]]) ^ (LOOKUP_MUL_9[state[(4*i)+1]]) 
                ^ (LOOKUP_MUL_14[state[(4*i)+2]]) ^ (LOOKUP_MUL_11[state[(4*i)+3]]);
            temp[3] = (LOOKUP_MUL_11[state[(4*i)+0]]) ^ (LOOKUP_MUL_13[state[(4*i)+1]]) 
                ^ (LOOKUP_MUL_9[state[(4*i)+2]]) ^ (LOOKUP_MUL_14[state[(4*i)+3]]);
            state[(4*i)+0] = temp[0];
            state[(4*i)+1] = temp[1];
            state[(4*i)+2] = temp[2];
            state[(4*i)+3] = temp[3];
        }
    }
    else                        //for encryption
    {
        for(int i=0;i<4;i++)
        {
            temp[0] = (LOOKUP_MUL_2[state[(4*i)+0]]) ^ (LOOKUP_MUL_3[state[(4*i)+1]]) 
                ^ (state[(4*i)+2]) ^ (state[(4*i)+3]);
            temp[1] = (state[(4*i)+0]) ^ (LOOKUP_MUL_2[state[(4*i)+1]])
                ^ (LOOKUP_MUL_3[state[(4*i)+2]]) ^ (state[(4*i)+3]);
            temp[2] = (state[(4*i)+0]) ^ (state[(4*i)+1]) 
                ^ (LOOKUP_MUL_2[state[(4*i)+2]]) ^ (LOOKUP_MUL_3[state[(4*i)+3]]);
            temp[3] = (LOOKUP_MUL_3[state[(4*i)+0]]) ^ (state[(4*i)+1]) 
                ^ (state[(4*i)+2]) ^ (LOOKUP_MUL_2[state[(4*i)+3]]);
            state[(4*i)+0] = temp[0];
            state[(4*i)+1] = temp[1];
            state[(4*i)+2] = temp[2];
            state[(4*i)+3] = temp[3];
        }
    }
}

void AES256::shiftRows(unsigned char state[], bool inverse)
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