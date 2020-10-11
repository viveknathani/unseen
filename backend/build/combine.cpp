#include <iostream>
#include <fstream>
#include "../lib/Image.hpp"
#include "../algorithm/AES256.hpp"
#include "../algorithm/AESLookups.hpp"

unsigned int getNumberOfBytes(std::string filePath)
{
    unsigned int start, end;
    std::ifstream fileReader;
    fileReader.open(filePath, std::ios::binary | std::ios::in);
    fileReader.seekg(0, std::ios::beg);
    start = fileReader.tellg();
    fileReader.seekg(0, std::ios::end);
    end = fileReader.tellg();
    fileReader.close();
    return (end - start);
}

/*
    argv[] has to be an array of length 4
    arg[0] = name of object file
    arg[1] = path to input image
    arg[2] = path to output image
    arg[3] = key hexinput
    arg[4] = task (encrypt/decrypt) (0/1)
    arg[5] = path to binary file for decryption    
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
    Pixel** matrix = img.getRGBMatrix();            //matrix to store the values of input image
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

    if(formatTask == 1)
    {
        std::string filePath = argv[5];
        std::ifstream fileReader;
        int n = getNumberOfBytes(filePath);
        unsigned char finalBuffer[n];
        char tempBuffer[n];
        fileReader.open(filePath, std::ios::binary | std::ios::in);
        fileReader.read(tempBuffer, n);
        fileReader.close();
        for(int i = 0; i < n; i++)
            finalBuffer[i] = (unsigned char)tempBuffer[i];
        std::string IV;
        for(unsigned int i =0;i < 16;i++)
        {
            IV += LOOKUP_TO_HEX[finalBuffer[i]];
        }
        for(unsigned int i = 16;i < n;i++)                  //add the .dat info to the input
        {
            hexinput +=LOOKUP_TO_HEX[finalBuffer[i]];
        }
        AES256 object(hexinput,key,IV,1,false);
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
        Image decryptedImg(1,width,height,outputMatrix,output);

        for(unsigned int i = 0; i < height; i++)
            free(outputMatrix[i]);
        free(outputMatrix);
    }

    if(formatTask==0)
    {
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

        std::vector<unsigned char>IVByte = object.getIVBytes();
        std::string filePath = "temp.dat";
    
        std::ofstream fileWriter;
        unsigned char buffer[16+((hexoutput.length()-width*height*3)/2)];
        for(unsigned int i=0;i<16;i++)
        {
            buffer[i] = IVByte[i];
        }
        for(unsigned int i=16; i<(hexoutput.length()-width*height*3)/2;i=i+2)
        {
            std::string buf;
            buf = hexoutput[(width*height*3)+i];
            buf +=hexoutput[(width*height*3)+i+1];
            buffer[i]=LOOKUP_TO_BYTE.at(buf);
        }
        fileWriter.open(filePath, std::ios::binary | std::ios::out);
        fileWriter.write((char *)buffer, 16+((hexoutput.length()-width*height*3)/2));
        fileWriter.close();
    }
    return 0;
}