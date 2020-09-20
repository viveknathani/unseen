#include "Image.hpp"
#include <string>
#include <cstring>
#include <jpeglib.h>
#include <png.h>
#include <iostream>

#define show(x) std::cout << #x << " = " << x << std::endl

Image::Image(int givenFiletype, unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix)
{
    switch(givenFiletype)
    {
        case 0: writeJPG(givenWidth, givenHeight, givenMatrix);
                break;

        case 1: writePNG(givenWidth, givenHeight, givenMatrix);
                break;
        default: std::cout << "Cannot make an image of this filetype." << std::endl;               
    }
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
                matrix[i][j].r = (unsigned char)(px[0]);
                matrix[i][j].g = (unsigned char)(px[1]);
                matrix[i][j].b = (unsigned char)(px[2]);
            }
        }
        free(rowPointers);
    }

}

void Image::writeJPG(unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix)
{
    int code = 0;
    FILE *fp = NULL;
    JSAMPLE * image_buffer;
    int image_height = givenHeight, image_width = givenWidth;
    fp = fopen("output.jpg", "wb");
    if (fp == NULL) {
        std::cout << "fopen error" << std::endl;
    }
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = image_width; 	
    cinfo.image_height = image_height;
    cinfo.input_components = 3;	
    cinfo.in_color_space = JCS_RGB; 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = image_width * 3;
    int p = 0;
    image_buffer = (JSAMPLE *)malloc(3 * image_width * image_height);
    for(int i = 0; i < image_height; i++)
    {
        for(int j = 0; j < image_width; j++)
        {
            *(image_buffer + p) = givenMatrix[i][j].r;
            p++;
            *(image_buffer + p) = givenMatrix[i][j].g;
            p++;
            *(image_buffer + p) = givenMatrix[i][j].b;
            p++;
        }
    }

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);
    this->filetype = 1;
    this->width = 1;
    this->height = 1;
    this->matrix = (Pixel **)malloc(this->height * sizeof(Pixel *));
        for(unsigned int i = 0; i < this->height; i++)
            matrix[i] = (Pixel *)malloc(this->width * sizeof(Pixel));

}

void Image::writePNG(unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix)
{
    FILE *file = NULL;
    int code = 0;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    fp = fopen("output.png", "wb");
    if (fp == NULL) {
        std::cout << "fopen error" << std::endl;
    }
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
      fprintf(stderr, "Error during png creation\n");
      code = 1;
   }

    png_init_io(png_ptr, fp);

   // Write header (8 bit colour depth)
   png_set_IHDR(png_ptr, info_ptr, givenWidth, givenHeight,
         8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);  
    png_write_info(png_ptr, info_ptr);

    png_bytep *rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * givenHeight);
        for(unsigned int i = 0; i < givenHeight; i++) 
            rowPointers[i] = (png_byte*)malloc(png_get_rowbytes(png_ptr,info_ptr));

   // Write image data
   int x, y;
   for (y=0 ; y<givenHeight ; y++) {
      png_bytep row = rowPointers[y];
      for (x=0 ; x<givenWidth; x++) {
        //unsigned char r = 255, g = 0, b = 0;  
        png_bytep px = &(row[x * 3]);
        px[0] = (png_byte)givenMatrix[y][x].r;
        px[1] = (png_byte)givenMatrix[y][x].g;
        px[2] = (png_byte)givenMatrix[y][x].b;
      }
   }
    png_set_rows (png_ptr, info_ptr, rowPointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
   // End write
   png_write_end(png_ptr, NULL); 
   this->filetype = 1;
   this->width = 1;
   this->height = 1;
   this->matrix = (Pixel **)malloc(this->height * sizeof(Pixel *));
    for(unsigned int i = 0; i < this->height; i++)
        matrix[i] = (Pixel *)malloc(this->width * sizeof(Pixel));
}

Pixel** Image::getRGBMatrix()
{
    return matrix;
}

Image::~Image()
{
   free(matrix);
}

int main()
{
    Image img("sample.jpg");
    Pixel **testMatrix = img.getRGBMatrix();
    Image two(0, img.getWidth(), img.getHeight(), testMatrix);
    return 0;
}