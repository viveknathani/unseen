#include "AES256.hpp"
#include "AESLookups.hpp"
#include "GetIV.hpp"
#include <string>
#include <vector>
#include <iostream>
using namespace std;
#define show(x) cout << #x << " = " << x << endl
AES256::AES256(std::string hexText, std::string hexKey)
{
    this->hexText = hexText;
    std::string iv = getIV();
    convertHexToBytes(hexKey, byteKey);
    //convertHexToBytes(iv, byteIV);
    keyExpansion(byteKey);
}

AES256::AES256(std::string hexText, std::string hexKey, std::string hexIV)
{

}

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

void AES256::subBytes(unsigned char state[], bool inverse)
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

void AES256::subWord(unsigned char word[], unsigned char result[])
{
    for(int i = 0; i < WORD_SIZE_IN_BYTES; i++)   
            result[i] = LOOKUP_SBOX[word[i]];    
}

void AES256::rotWord(unsigned char word[], unsigned char result[])
{
    unsigned char temp[WORD_SIZE_IN_BYTES];    
    temp[0] = word[1]; 
    temp[1] = word[2];
    temp[2] = word[3];
    temp[3] = word[0];

    for(int i = 0; i < WORD_SIZE_IN_BYTES; i++)
        result[i] = temp[i];
}

void AES256::addRoundKey(unsigned char state[], int index)
{
    unsigned char currentRoundKey[ROUND_KEY_SIZE_IN_BYTES];
    for(int i = 0; i < ROUND_KEY_SIZE_IN_BYTES; i++)
        currentRoundKey[i] = static_cast<unsigned char>(roundKeys[index][i]);

    for(int i = 0; i < BLOCK_SIZE_IN_BYTES; i++)
        state[i] = state[i] ^ currentRoundKey[i];
}

void AES256::xorWords(unsigned char one[], unsigned char two[], unsigned char result[])
{
    for(int i = 0; i < WORD_SIZE_IN_BYTES; i++)
        result[i] = one[i] ^ two[i];
}

void AES256::keyExpansion(unsigned char key[])
{
    unsigned char wordsInKey[NUMBER_OF_WORDS_IN_KEY][WORD_SIZE_IN_BYTES + 1];

    for(int i = 0; i < NUMBER_OF_WORDS_IN_KEY; i++)
    {
        wordsInKey[i][0] = key[4*i];
        wordsInKey[i][1] = key[4*i+1];
        wordsInKey[i][2] = key[4*i+2];
        wordsInKey[i][3] = key[4*i+3];
        wordsInKey[i][4] = '\0';
    }

    for(int i = 0; i < NUMBER_OF_WORDS; i++)
    {
        if(i < NUMBER_OF_WORDS_IN_KEY)
        {
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                words[i][j] = wordsInKey[i][j];
            words[i][WORD_SIZE_IN_BYTES] = '\0';    
        }
        else if(i >= NUMBER_OF_WORDS_IN_KEY && (i % NUMBER_OF_WORDS_IN_KEY == 0))
        {
            unsigned char one[WORD_SIZE_IN_BYTES];
            unsigned char two[WORD_SIZE_IN_BYTES];
            unsigned char rconword[WORD_SIZE_IN_BYTES];
            unsigned char result[WORD_SIZE_IN_BYTES];
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                one[j] = words[i - NUMBER_OF_WORDS_IN_KEY][j];
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                two[j] = words[i - 1][j];   

            rotWord(two, two);    
            subWord(two, two);

            int index = (i / NUMBER_OF_WORDS_IN_KEY);
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                rconword[j] = LOOKUP_RCON[index-1][j];

            xorWords(one, two, result);
            xorWords(result, rconword, result);

            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                words[i][j] = result[j];
            words[i][WORD_SIZE_IN_BYTES] = '\0'; 
        }

        else if(i >= NUMBER_OF_WORDS_IN_KEY && (i % NUMBER_OF_WORDS_IN_KEY == 4))
        {
            unsigned char one[WORD_SIZE_IN_BYTES];
            unsigned char two[WORD_SIZE_IN_BYTES];
            unsigned char result[WORD_SIZE_IN_BYTES];   
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                one[j] = words[i - NUMBER_OF_WORDS_IN_KEY][j];
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                two[j] = words[i - 1][j];    

            subWord(two, two);       

            xorWords(one, two, result);
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                words[i][j] = result[j];
            words[i][WORD_SIZE_IN_BYTES] = '\0'; 
        }
        else 
        {
            unsigned char one[WORD_SIZE_IN_BYTES];
            unsigned char two[WORD_SIZE_IN_BYTES];
            unsigned char result[WORD_SIZE_IN_BYTES];   
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                one[j] = static_cast<unsigned char>(words[i - NUMBER_OF_WORDS_IN_KEY][j]);
            for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                two[j] = static_cast<unsigned char>(words[i - 1][j]);  

            xorWords(one, two, result);         

           for(int j = 0; j < WORD_SIZE_IN_BYTES; j++)
                words[i][j] = result[j];
            words[i][WORD_SIZE_IN_BYTES] = '\0'; 
        }
    }
}

void AES256::convertHexToBytes(std::string str, unsigned char byteArray[])
{
    int bytePointer = 0;
    for(int i = 0; i < str.size(); i += 2)
    {
        std::string input;
        input += str[i];
        input += str[i+1];
        byteArray[bytePointer] = LOOKUP_TO_BYTE.at(input);
        bytePointer++;
    }
}

std::vector<std::vector<unsigned char>> AES256::getAllWords()
{
    std::vector<std::vector<unsigned char>> keysToSend(NUMBER_OF_WORDS);
    for(int i = 0; i < NUMBER_OF_WORDS; i++)
    {
        std::vector<unsigned char> temp(4);
        for(int j = 0; j < 4; j++)
        {
            temp[j] = words[i][j];
        }
        keysToSend[i] = temp;
    }
    return keysToSend;    
}

int main()
{
    AES256 obj("11", "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4");
    std::vector<std::vector<unsigned char>> allWords = obj.getAllWords();
    cout << "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4" << endl;
    for(int i = 0; i < (int)allWords.size(); i++)
    {
        for(int j = 0; j < (int)allWords[i].size(); j++)
        {
            std::cout << LOOKUP_TO_HEX[allWords[i][j]];
        }
        std::cout << std::endl;
    }
    return 0;
}