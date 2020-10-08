#include "AES256.hpp"
#include "AESLookups.hpp"
#include "GetIV.hpp"
#include <string>
#include <vector>
#include <iostream>

AES256::AES256(std::string hexInput, std::string hexKey, std::string hexIV, int task)
{
    this->originalLength = hexInput.size();
    this->hexInput = hexInput;

    for(int i = 0; i < KEY_SIZE_IN_BYTES; i++)
        byteKey.push_back(0);

    for(int i = 0; i < IV_SIZE_IN_BYTES; i++)
        byteIV.push_back(0);

    for(int i = 0; i < NUMBER_OF_ROUNDS; i++)
    {
        std::vector<unsigned char> temp(ROUND_KEY_SIZE_IN_BYTES);
        roundKeys.push_back(temp);
    }

    for(int i = 0; i < NUMBER_OF_WORDS; i++)
    {
        std::vector<unsigned char> temp(WORD_SIZE_IN_BYTES);
        words.push_back(temp);
    }

    convertHexToBytes(hexKey, byteKey);
    keyExpansion(byteKey);

    switch(task)
    {
        case ENCRYPT : {
            hexIV = getIV();
            convertHexToBytes(hexIV, byteIV);
            encrypt();
            break;
        }

        case DECRYPT: {
            convertHexToBytes(hexIV, byteIV);
            break;
        }

        default : std::cout << "Invalid task number." << std::endl;
    }
}

void AES256::encrypt()
{

}

void AES256::decrypt()
{

}

void AES256::keyExpansion(std::vector<unsigned char> &key)
{
    std::vector<std::vector<unsigned char>> wordsInKey(NUMBER_OF_WORDS_IN_KEY);
    for(int i = 0; i < NUMBER_OF_WORDS_IN_KEY; i++)
    {
        std::vector<unsigned char> temp(WORD_SIZE_IN_BYTES);
        for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
            temp[j] = key[(4*i) + j];
        wordsInKey[i] = temp;    
    }

    for(int i = 0; i < NUMBER_OF_WORDS; i++)
    {
        if(i < NUMBER_OF_WORDS_IN_KEY)
        {
            words[i] = wordsInKey[i];   
        }
        else if(i >= NUMBER_OF_WORDS_IN_KEY && (i % NUMBER_OF_WORDS_IN_KEY == 0))
        {
            std::vector<unsigned char> rconWord(WORD_SIZE_IN_BYTES);

            int index = (i / NUMBER_OF_WORDS_IN_KEY);
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                rconWord[j] = LOOKUP_RCON[index-1][j];

            words[i] = xorWords( 
                            xorWords(
                                words[i - NUMBER_OF_WORDS_IN_KEY], 
                                subWord(rotWord(words[i-1]))
                                    ), 
                            rconWord
                        );
        }

        else if(i >= NUMBER_OF_WORDS_IN_KEY && (i % NUMBER_OF_WORDS_IN_KEY == 4))
        {
            words[i] = xorWords(
                            words[i - NUMBER_OF_WORDS_IN_KEY], 
                            subWord(words[i-1])
                       );
        }
        else 
        {
            words[i] = xorWords(
                            words[i - NUMBER_OF_WORDS_IN_KEY], 
                            words[i-1]
                       );
        }
    }

    int rKeyPtr = 0;
    for(int i = 0; i < NUMBER_OF_WORDS; i += 4)
    {
        int keyPtr = 0;
        for(int j = i; j < i+4; j++)
        {
            for(int k = 0; k < WORD_SIZE_IN_BYTES; k++)
            {
                roundKeys[rKeyPtr][keyPtr] = words[j][k];
                keyPtr++;
            }
        }
        rKeyPtr++;
    }
}

void AES256::subBytes(std::vector<unsigned char> &state, bool inverse)
{
    if(inverse)
    {
        for(int i = 0; i < BLOCK_SIZE_IN_BYTES; i++)
            state[i] = LOOKUP_INV_SBOX[state[i]];
    }
    else 
    {
        for(int i = 0; i < BLOCK_SIZE_IN_BYTES; i++)
            state[i] = LOOKUP_SBOX[state[i]];
    }
}

void AES256::mixColumns(std::vector<unsigned char> &state, bool inverse)
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

void AES256::shiftRows(std::vector<unsigned char> &state, bool inverse)
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

void AES256::addRoundKey(std::vector<unsigned char> &state, int index)
{
    for(int i = 0; i < BLOCK_SIZE_IN_BYTES; i++)
        state[i] = state[i] ^ roundKeys[index][i];
}

std::vector<unsigned char> AES256::subWord(std::vector<unsigned char> word)
{
    std::vector<unsigned char> result(WORD_SIZE_IN_BYTES);
    for(int i = 0; i < WORD_SIZE_IN_BYTES; i++)   
            result[i] = LOOKUP_SBOX[word[i]];    
    return result;        
}

std::vector<unsigned char> AES256::rotWord(std::vector<unsigned char> word)
{
    std::vector<unsigned char> result(WORD_SIZE_IN_BYTES);

    result[0] = word[1]; 
    result[1] = word[2];
    result[2] = word[3];
    result[3] = word[0];

    return result;
}

std::vector<unsigned char> AES256::xorWords(std::vector<unsigned char> one, std::vector<unsigned char> two)
{
    std::vector<unsigned char> result(WORD_SIZE_IN_BYTES);   
    for(int i = 0; i < WORD_SIZE_IN_BYTES; i++)
        result[i] = one[i] ^ two[i];
    return result;    
}

void AES256::convertHexToBytes(std::string str, std::vector<unsigned char> &byteVec)
{
    int bytePointer = 0;
    for(int i = 0; i < str.size(); i += 2)
    {
        std::string input;
        input += str[i];
        input += str[i+1];
        byteVec[bytePointer] = LOOKUP_TO_BYTE.at(input);
        bytePointer++;
    }
}

std::vector<std::vector<unsigned char>> AES256::getAllWords()
{
    return words;
}

std::vector<std::vector<unsigned char>> AES256::getAllRoundKeys()
{
    return roundKeys;
}

std::string AES256::getHexOutput()
{
    return hexOutput;
}