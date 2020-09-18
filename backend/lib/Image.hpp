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

    public :
        Image(unsigned int filetype, unsigned int width, unsigned int height);
        Image(std::string path);  
        int findFileType(std::string path);
        void readJPGAndFillMatrix(std::string path);
        void readPNGAndFillMatrix(std::string path);
        Pixel** getRGBMatrix();
        unsigned int getHeight() { return height; }
        unsigned int getWidth() { return width; }
        ~Image();
};

#endif