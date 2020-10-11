#include "../lib/Image.hpp"
#include "../algorithm/AES256.hpp"
#include "../algorithm/AESLookups.hpp"
#include <fstream>
#include <iostream>
#include <vector>

int getNumberOfBytes(std::string filePath);

/*
    argv[] has to be an array of length 5 or 6

    argv[0] = name of object file.
    argv[1] = path to input image.
    argv[2] = path to output image.
    argv[3] = key in hex.
    argv[4] = task (encrypt/decrypt) (0/1).
    argv[5] = path to binary file for decryption (this is case specific)

    arg[0]'s existence is irrelevant to our needs.
    Hence it is not extracted.
*/

int main(int argc, char** argv)
{
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    std::string hexKey = argv[3];
    std::string task = argv[4];
    int formatTask = std::stoi(task);

    if(!(formatTask == 0 || formatTask == 1))
    {
        std::cout << "Invalid task format." << std::endl;
        return 1;
    }

    if((int)hexKey.length() != 64)
    {
        std::cout << "Invalid key provided." << std::endl;
        return 1;
    }


    std::string hexInput;
    Image inputImage(inputPath);
    Pixel** imageMatrix = inputImage.getRGBMatrix();
    unsigned int width = inputImage.getWidth(), height = inputImage.getHeight();

    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            hexInput += LOOKUP_TO_HEX[imageMatrix[i][j].r];
            hexInput += LOOKUP_TO_HEX[imageMatrix[i][j].g];
            hexInput += LOOKUP_TO_HEX[imageMatrix[i][j].b];
        }
    }

    switch(formatTask)
    {
        case 0: {
            std::string hexIV = "00";
            AES256 object(hexInput, hexKey, hexIV, ENCRYPT, true);
            std::string hexOutput = object.getHexOutput();

            int hexPointer = 0;
            for(unsigned int i = 0; i < height; i++)
            {
                for(unsigned int j = 0; j < width; j++)
                {
                    std::string tempR, tempG, tempB;
                    tempR += hexOutput[hexPointer];
                    hexPointer++;
                    tempR += hexOutput[hexPointer];
                    hexPointer++;
                    tempG += hexOutput[hexPointer];
                    hexPointer++;
                    tempG += hexOutput[hexPointer];
                    hexPointer++;
                    tempB += hexOutput[hexPointer];
                    hexPointer++;
                    tempB += hexOutput[hexPointer];
                    hexPointer++;

                    imageMatrix[i][j].r = LOOKUP_TO_BYTE.at(tempR);
                    imageMatrix[i][j].g = LOOKUP_TO_BYTE.at(tempG);
                    imageMatrix[i][j].b = LOOKUP_TO_BYTE.at(tempB);
                }
            }

            int extraBytes = ((int)hexOutput.length() - (height * width * 3 * 2))/2;
            int bufferSize = IV_SIZE_IN_BYTES + extraBytes;
            unsigned char buffer[bufferSize];
            std::vector<unsigned char> byteIV = object.getByteIV();
            for(int i = 0; i < IV_SIZE_IN_BYTES; i++)
                buffer[i] = byteIV[i];
            for(int i = 16; i < bufferSize; i++)
            {
                std::string tempBuffer;
                tempBuffer += hexOutput[hexPointer];
                hexPointer++;
                tempBuffer += hexOutput[hexPointer];
                hexPointer++;
                buffer[i] = LOOKUP_TO_BYTE.at(tempBuffer);
            }    

            std::string binaryFilePath = "../bin/temp.dat";
            std::ofstream fileWriter;
            fileWriter.open(binaryFilePath, std::ios::binary | std::ios::out);
            fileWriter.write((char *)buffer, bufferSize);
            fileWriter.close();

            Image encryptedImage(1, width, height, imageMatrix, outputPath);
            break;
        }
        
        case 1: {
            std::string binaryFilePath = argv[5];
            int bytesToRead = getNumberOfBytes(binaryFilePath);

            unsigned char buffer[bytesToRead];
            char tempBuffer[bytesToRead];
            std::ifstream fileReader;
            fileReader.open(binaryFilePath, std::ios::binary | std::ios::in);
            fileReader.read(tempBuffer, bytesToRead);
            fileReader.close();
            for(int i = 0; i < bytesToRead; i++)
                buffer[i] = (unsigned char)tempBuffer[i];

            for(int i = IV_SIZE_IN_BYTES; i < bytesToRead; i++)
                hexInput += LOOKUP_TO_HEX[buffer[i]];

            std::string hexIV;
            for(int i = 0; i < IV_SIZE_IN_BYTES; i++)
                hexIV += LOOKUP_TO_HEX[buffer[i]];

            AES256 object(hexInput, hexKey, hexIV, DECRYPT, false);    
            std::string hexOutput = object.getHexOutput();

            int hexPointer = 0;
            for(unsigned int i = 0; i < height; i++)
            {
                for(unsigned int j = 0; j < width; j++)
                {
                    std::string tempR, tempG, tempB;
                    tempR += hexOutput[hexPointer];
                    hexPointer++;
                    tempR += hexOutput[hexPointer];
                    hexPointer++;
                    tempG += hexOutput[hexPointer];
                    hexPointer++;
                    tempG += hexOutput[hexPointer];
                    hexPointer++;
                    tempB += hexOutput[hexPointer];
                    hexPointer++;
                    tempB += hexOutput[hexPointer];
                    hexPointer++;

                    imageMatrix[i][j].r = LOOKUP_TO_BYTE.at(tempR);
                    imageMatrix[i][j].g = LOOKUP_TO_BYTE.at(tempG);
                    imageMatrix[i][j].b = LOOKUP_TO_BYTE.at(tempB);
                }
            }

            Image decryptedImage(1, width, height, imageMatrix, outputPath);
            break;
        }
    }
    return 0;
}

int getNumberOfBytes(std::string filePath)
{
    int start, end;
    std::ifstream fileReader;
    fileReader.open(filePath, std::ios::binary | std::ios::in);
    fileReader.seekg(0, std::ios::beg);
    start = fileReader.tellg();
    fileReader.seekg(0, std::ios::end);
    end = fileReader.tellg();
    fileReader.close();
    return (end - start);
}