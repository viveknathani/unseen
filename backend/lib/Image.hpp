#ifndef UNSEEN_IMAGE_H
#define UNSEEN_IMAGE_H

#include <string>

struct Pixel 
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class Image 
{
    private :
        unsigned int width;
        unsigned int height;
        int filetype;
        Pixel **matrix;
        bool isNotACopy;

    public :
        Image(int givenFiletype, unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix, std::string output);
        Image(std::string path);  
        int findFileType(std::string path);
        void readJPGAndFillMatrix(std::string path);
        void readPNGAndFillMatrix(std::string path);
        void writeJPG(unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix, std::string output);
        void writePNG(unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix, std::string output);
        Pixel** getRGBMatrix();
        unsigned int getHeight() { return height; }
        unsigned int getWidth() { return width; }
        ~Image();
};

#endif