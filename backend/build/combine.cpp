#include <iostream>
#include "../lib/Image.hpp"
#include "../algorithm/AES256.hpp"
#include "../algorithm/AESLookups.hpp"

/*
    argv[] has to be an array of length 4
    arg[0] = name of object file
    arg[1] = path to input image
    arg[2] = path to output image
    arg[3] = key hexinput
    arg[4] = task (encrypt/decrypt) (0/1)
    
    arg[0]'s existence is irrelevant to the main function.
    Hence it is not extracted.
*/

int main(int argc, char** argv)
{
    // extracting the arguments
    std::string input = argv[1];
    std::string output = argv[2];
    std::string key = argv[3];
    std::string task = argv[4];
    int formatTask = std::stoi(task);
    
    if(!(formatTask == 0 || formatTask == 1))
    {
        std::cout << "Invalid task format." << std::endl;
        return 1;
    }

    if((int)key.length() != 64)
    {
        std::cout << "Invalid key provided." << std::endl;
        return 1;
    }

    std::string hexinput;
    Image img(input);
    Pixel** matrix = img.getRGBMatrix();
    unsigned int width = img.getWidth();
    unsigned int height = img.getHeight();
    for(unsigned int i = 0;i < height;i++)
    {
        for(unsigned int j = 0;j < width;j++)
        {
            hexinput += LOOKUP_TO_HEX[matrix[i][j].r];
            hexinput += LOOKUP_TO_HEX[matrix[i][j].g];
            hexinput += LOOKUP_TO_HEX[matrix[i][j].b];
        }
    }

    std::string IV = "00";

    AES256 object(hexinput,key,IV,0,true);

    std::string hexoutput = object.getHexOutput();

    Pixel **outputMatrix;
    outputMatrix = (Pixel **)malloc(height * sizeof(Pixel *));
    for(unsigned int i = 0; i < height; i++)
        outputMatrix[i] = (Pixel *)malloc(width * sizeof(Pixel));

    int ptr = 0;
    for(unsigned int i = 0;i < height;i++)
    {
        for(unsigned int j = 0;j < width;j++)
        {
            std::string outputToByteR;
            outputToByteR = hexoutput[ptr];
            outputToByteR += hexoutput[ptr+1];
            ptr=ptr+2;
            outputMatrix[i][j].r=LOOKUP_TO_BYTE.at(outputToByteR);
            std::string outputToByteG;
            outputToByteG = hexoutput[ptr];
            outputToByteG += hexoutput[ptr+1];
            ptr=ptr+2;
            outputMatrix[i][j].g=LOOKUP_TO_BYTE.at(outputToByteG);
            std::string outputToByteB;
            outputToByteB = hexoutput[ptr];
            outputToByteB += hexoutput[ptr+1];
            ptr=ptr+2;
            outputMatrix[i][j].b=LOOKUP_TO_BYTE.at(outputToByteB);
        }
    }

    Image newEncryptedImg(1,width,height,outputMatrix,output);

    for(unsigned int i = 0; i < height; i++)
        free(outputMatrix[i]);
    free(outputMatrix);
    

    return 0;
}