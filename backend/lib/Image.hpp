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
        unsigned int filetype;
        struct Pixel **matrix;

    public :
        Image(unsigned int filetype, unsigned int width, unsigned int height);
        Image(std::string path);  
        ~Image();
};

#endif