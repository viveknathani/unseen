#include "Image.hpp"
#include <string>
#include <cstring>
#include <jpeglib.h>
#include <png.h>
#include <iostream>

#define show(x) std::cout << #x << " = " << x << std::endl

Image::Image(unsigned int filetype, unsigned int width, unsigned int height)
{

}

Image::Image(std::string path)
{
    filetype = findFileType(path);
    if(filetype == -1)
    {
        std::cout << "Cannot process this file." << std::endl;
    }
    else 
    {
        switch(filetype)
        {
            case 0: readJPGAndFillMatrix(path);
                    break;
            case 1: readPNGAndFillMatrix(path);
                    break;
            default: std::cout << "Unhandled filetype." << std::endl;                
        }
    }
}

int Image::findFileType(std::string path)
{
    FILE *file;
    unsigned char buffer[8];
    memset(buffer, 0, sizeof(buffer));
    int jpgSignature[] = {255, 216, 255}; 
    int pngSignature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    bool checkJPG = true, checkPNG = true;

    if((file = fopen(path.c_str(), "rb")) == nullptr)
    {
        std::cout << "Error in reading the file." << std::endl;
        return -1;
    }
    size_t readBytes = fread(buffer, sizeof(buffer), 1, file);
    fclose(file);

    for(int i = 0; i < 3; i++)
        if((int)buffer[i] != jpgSignature[i]) checkJPG = false;
    for(int i = 0; i < 8; i++)
        if((int)buffer[i] != pngSignature[i]) checkPNG = false;   

    if(checkJPG) return 0;
    else if(checkPNG) return 1;
    else return -1;
}

void Image::readJPGAndFillMatrix(std::string path)
{
    unsigned int rowStride, noOfChannels;
    unsigned char *bmpBuffer;
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    FILE *file;
    if((file = fopen(path.c_str(), "rb")) == NULL)
    {
        std::cout << "Error in reading the file." << std::endl;
    }
    else 
    {
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, file);
        int unprocessedValue = jpeg_read_header(&cinfo, TRUE);
        jpeg_start_decompress(&cinfo);
        width = static_cast<unsigned int>(cinfo.output_width);
        height = static_cast<unsigned int>(cinfo.output_height);
        noOfChannels = static_cast<unsigned int>(cinfo.output_components);
        bmpBuffer = (unsigned char *)malloc(width * height * noOfChannels);
        matrix = (Pixel **)malloc(height * sizeof(Pixel *));
        for(unsigned int i = 0; i < height; i++)
            matrix[i] = (Pixel *)malloc(width * sizeof(Pixel));
        rowStride = this->width * noOfChannels;
        int x = 0, y = 0;
        while(cinfo.output_scanline < cinfo.output_height)
        {
            unsigned char *rowBuffer[1];
            rowBuffer[0] = bmpBuffer + ((cinfo.output_scanline) * static_cast<int>(rowStride));
            jpeg_read_scanlines(&cinfo, rowBuffer, 1);
            
            for(unsigned int i = 0; i < width; i++)
            {
                matrix[x][y].r = (unsigned char)(*rowBuffer[0]++);
                matrix[x][y].g = (unsigned char)(*rowBuffer[0]++);
                matrix[x][y].b = (unsigned char)(*rowBuffer[0]++);
                y++;
            }
            x++;
            y = 0;
        }
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        free(bmpBuffer);
        fclose(file);
      }
}

void Image::readPNGAndFillMatrix(std::string path)
{
    FILE *file;
    if((file = fopen(path.c_str(), "rb")) == NULL)
    {
        std::cout << "Error in reading the file." << std::endl;
    }
    else 
    {
        png_byte colorType, bitDepth;
        png_bytep *rowPointers;
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if(!png) abort();
        png_infop info = png_create_info_struct(png);
        if(!info) abort();
        if(setjmp(png_jmpbuf(png))) abort();
        png_init_io(png, file);
        png_read_info(png, info);
        width = static_cast<unsigned int>(png_get_image_width(png, info));
        height = static_cast<unsigned int>(png_get_image_height(png, info));
        matrix = (Pixel **)malloc(height * sizeof(Pixel *));
        for(unsigned int i = 0; i < height; i++)
            matrix[i] = (Pixel *)malloc(width * sizeof(Pixel));
        colorType = png_get_color_type(png, info);
        bitDepth  = png_get_bit_depth(png, info);

        if(bitDepth == 16) png_set_strip_16(png);
        if(colorType == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
        if(colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) png_set_expand_gray_1_2_4_to_8(png);
        if(png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);

        if(colorType == PNG_COLOR_TYPE_RGB ||
           colorType == PNG_COLOR_TYPE_GRAY ||
           colorType == PNG_COLOR_TYPE_PALETTE)
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
        if(colorType == PNG_COLOR_TYPE_GRAY ||
           colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);   

        png_read_update_info(png, info);

        rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
        for(unsigned int i = 0; i < height; i++) 
            rowPointers[i] = (png_byte*)malloc(png_get_rowbytes(png,info));

        png_read_image(png, rowPointers);

        for(unsigned int i = 0; i < height; i++)
        {
            png_bytep row = rowPointers[i];
            for(unsigned int j = 0; j < width; j++)
            {
                png_bytep px = &(row[j * 4]);
                matrix[i][j].r = (int)(px[0]);
                matrix[i][j].g = (unsigned char)(px[1]);
                matrix[i][j].b = (unsigned char)(px[2]);
            }
        }
        free(rowPointers);
    }

}

Pixel** Image::getRGBMatrix()
{
    return matrix;
}

Image::~Image()
{
   free(matrix);
}