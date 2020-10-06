#ifndef AES256_H
#define AES256_H

#include <string>

#define ENCRYPT 0
#define DECRYPT 1
#define WORD_SIZE_IN_BYTES 4
#define BLOCK_SIZE_IN_BYTES 16
#define KEY_SIZE_IN_BYTES 32
#define ROUND_KEY_SIZE_IN_BYTES 16
#define NUMBER_OF_WORDS_IN_KEY 8
#define NUMBER_OF_ROUNDS 15
#define NUMBER_OF_WORDS 60

class AES256
{
    private :
        std::string hexText;
        std::string roundKeys[NUMBER_OF_ROUNDS];
        std::string words[NUMBER_OF_WORDS];
        unsigned char state[BLOCK_SIZE_IN_BYTES];

    public :

        // key functions
        AES256(std::string hexText, std::string hexKey, int task);    
        void keyExpansion(std::string key);
        void subBytes(unsigned char state[], bool inverse);
        void mixColumns(unsigned char state[], bool inverse);
        void shiftRows(unsigned char state[], bool inverse);
        void addRoundKey(unsigned char state[], int index);

        // utility functions
        void subWord(unsigned char word[], unsigned char result[]);
        void rotWord(unsigned char word[], unsigned char result[]);
        void xorWords(unsigned char one[], unsigned char two[], unsigned char result[]);
};

#endif
