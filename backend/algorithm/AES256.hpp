#ifndef AES256_H
#define AES256_H

#include <string>
#include <vector>

#define ENCRYPT 0
#define DECRYPT 1
#define WORD_SIZE_IN_BYTES 4
#define BLOCK_SIZE_IN_BYTES 16
#define IV_SIZE_IN_BYTES 16
#define KEY_SIZE_IN_BYTES 32
#define ROUND_KEY_SIZE_IN_BYTES 16
#define NUMBER_OF_WORDS_IN_KEY 8
#define NUMBER_OF_ROUNDS 15
#define NUMBER_OF_WORDS 60

class AES256
{
    private :
        unsigned int originalLength;
        std::string hexInput;
        std::string hexOutput;
        std::vector<unsigned char> byteKey;
        std::vector<unsigned char> byteIV;
        std::vector<std::vector<unsigned char>> roundKeys;
        std::vector<std::vector<unsigned char>> words;
        std::vector<std::vector<unsigned char>> blocks;

    public :
        AES256(std::string hexInput, std::string hexKey, std::string hexIV, int task);
        void encrypt();
        void decrypt();

        // key functions    
        void keyExpansion(std::vector<unsigned char> &key);
        void subBytes(std::vector<unsigned char> &state, bool inverse);
        void mixColumns(std::vector<unsigned char> &state, bool inverse);
        void shiftRows(std::vector<unsigned char> &state, bool inverse);
        void addRoundKey(std::vector<unsigned char> &state, int index);

        // utility functions
        std::vector<unsigned char> subWord(std::vector<unsigned char> word);
        std::vector<unsigned char> rotWord(std::vector<unsigned char> word);
        std::vector<unsigned char> xorWords(std::vector<unsigned char> one, std::vector<unsigned char> two);
        void convertHexToBytes(std::string str, std::vector<unsigned char> &byteVec);

        // getter functions
        std::vector<std::vector<unsigned char>> getAllWords();
        std::vector<std::vector<unsigned char>> getAllRoundKeys();
        std::string getHexOutput();
};

#endif
