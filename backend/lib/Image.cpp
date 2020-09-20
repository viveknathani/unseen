#include "Image.hpp"
#include <iostream>
#include <string>
#include <jpeglib.h>
#include <png.h>

/*
    Constructor that creates an image file with 
    properties specified by the user.
*/
Image::Image(int givenFiletype, unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix)
{
    // Making the file
    switch(givenFiletype)
    {
        case 0: writeJPG(givenWidth, givenHeight, givenMatrix);
                break;

        case 1: writePNG(givenWidth, givenHeight, givenMatrix);
                break;
        default: std::cout << "Cannot make an image of this filetype." << std::endl;               
    }

    // Assigning the user specified properties to the object.
    this->filetype = givenFiletype;
    this->width = givenWidth;
    this->height = givenHeight;

    // User already has the matrix, this is not needed. 
    this->matrix = NULL; 
}

/*
    Constructor that extracts the properties 
    from the image file whose path is given by the 
    user. The extracted properties are used in object creation.
*/
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

/*
    Detect file type using the signature.
    JPEG has the signature : FFD8FF (in hex)
    PNG has the signature  : 8950 4E47 0D0A 1A0A (in hex)
    For simplicity, we are using the decimal format while checking.

    Returns the following values :
    -1 : For unidentifiable signatures
     0 : JPEG format
     1 : PNG format
    
    More file types can be added in the future by performing
    checks on their signatures and mapping incremental return 
    values to their types.
*/
int Image::findFileType(std::string path)
{
    FILE *file;
    unsigned char buffer[8];    // the file will read the signature and store it in this buffer
    for(int i = 0; i < 8; i++)
        buffer[i] = 0;
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
        if(static_cast<int>(buffer[i]) != jpgSignature[i]) checkJPG = false;

    for(int i = 0; i < 8; i++)
        if(static_cast<int>(buffer[i]) != pngSignature[i]) checkPNG = false;   

    if(checkJPG) return 0;
    else if(checkPNG) return 1;
    else return -1;
}

/*
    Function that reads a jpeg file 
    and fills the RGB matrix in our object.
*/
void Image::readJPGAndFillMatrix(std::string path)
{
    unsigned int rowStride; // physical row width in imageBuffer 
    unsigned int noOfChannels; // number of colour components in the image
    unsigned char *imageBuffer; 
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    FILE *file;

    if((file = fopen(path.c_str(), "rb")) == NULL)
    {
        std::cout << "Error in reading the file." << std::endl;
        return ;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);

    int unprocessedValue = jpeg_read_header(&cinfo, TRUE);
    /* 
       We can ignore the return value from jpeg_read_header since
       (a) suspension is not possible with the stdio data source, and
       (b) we passed TRUE to reject a tables-only JPEG file as an error.
       See libjpeg.doc for more info.
    */

    jpeg_start_decompress(&cinfo);

    width = static_cast<unsigned int>(cinfo.output_width);
    height = static_cast<unsigned int>(cinfo.output_height);
    noOfChannels = static_cast<unsigned int>(cinfo.output_components);

    imageBuffer = (unsigned char *)malloc(width * height * noOfChannels);

    // allocate memory for our matrix
    matrix = (Pixel **)malloc(height * sizeof(Pixel *));
    for(unsigned int i = 0; i < height; i++)
        matrix[i] = (Pixel *)malloc(width * sizeof(Pixel));

    rowStride = width * noOfChannels;

    int x = 0, y = 0; // to traverse our matrix
    while(cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *rowBuffer[1];
        rowBuffer[0] = imageBuffer + ((cinfo.output_scanline) * static_cast<int>(rowStride));
        jpeg_read_scanlines(&cinfo, rowBuffer, 1);
        
        for(unsigned int i = 0; i < width; i++)
        {
            matrix[x][y].r = static_cast<unsigned char>(*rowBuffer[0]++);
            matrix[x][y].g = static_cast<unsigned char>(*rowBuffer[0]++);
            matrix[x][y].b = static_cast<unsigned char>(*rowBuffer[0]++);
            y++;
        }
        x++;
        y = 0;
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(imageBuffer);
    fclose(file);
}

/*
    Function that reads a png file 
    and fills the RGB matrix in our object.
*/
void Image::readPNGAndFillMatrix(std::string path)
{
    FILE *file;
    if((file = fopen(path.c_str(), "rb")) == NULL)
    {
        std::cout << "Error in reading the file." << std::endl;
        return ;
    }

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
            matrix[i][j].r = static_cast<unsigned char>(px[0]);
            matrix[i][j].g = static_cast<unsigned char>(px[1]);
            matrix[i][j].b = static_cast<unsigned char>(px[2]);
        }
    }
    free(rowPointers);
}

/*
   Function that writes a JPEG file with given properties.
*/
void Image::writeJPG(unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix)
{
    int code = 0;
    FILE *fp = NULL;
    JSAMPLE *imageBuffer;
    fp = fopen("output.jpg", "wb");
    if(fp == NULL) 
    {
        std::cout << "fopen error" << std::endl;
        return ;
    }

    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = givenWidth; 	
    cinfo.image_height = givenHeight;
    cinfo.input_components = 3;	
    cinfo.in_color_space = JCS_RGB; 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = givenWidth * 3;

    int p = 0; // for traversing imageBuffer
    imageBuffer = (JSAMPLE *)malloc(3 * givenWidth * givenHeight);

    // prepare buffer
    for(unsigned int i = 0; i < givenWidth; i++)
    {
        for(unsigned int j = 0; j < givenHeight; j++)
        {
            *(imageBuffer + p) = givenMatrix[i][j].r;
            p++;
            *(imageBuffer + p) = givenMatrix[i][j].g;
            p++;
            *(imageBuffer + p) = givenMatrix[i][j].b;
            p++;
        }
    }

    // write data
    while(cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = & imageBuffer[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);
}


/*
   Function that writes a PNG file with given properties.
*/
void Image::writePNG(unsigned int givenWidth, unsigned int givenHeight, Pixel **givenMatrix)
{
    FILE *file = NULL;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    fp = fopen("output.png", "wb");
    if (fp == NULL) 
    {
        std::cout << "fopen error" << std::endl;
        return ;
    }
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) 
    {
        std::cout << "Could not allocate write struct." << std::endl;
        return ;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) 
    {
       std::cout << "Could not allocate info struct." << std::endl;
       return ;
    }

    if(setjmp(png_jmpbuf(png_ptr))) 
    {
      std::cout << "Error during png creation." << std::endl;
      return ;
    }

    png_init_io(png_ptr, fp);

    // write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, givenWidth, givenHeight, 8, 
                PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);  
    png_write_info(png_ptr, info_ptr);

    png_bytep *rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * givenHeight);
        for(unsigned int i = 0; i < givenHeight; i++) 
            rowPointers[i] = (png_byte*)malloc(png_get_rowbytes(png_ptr,info_ptr));

    // write image data
    for(unsigned int i = 0 ; i < givenHeight ; i++) 
    {
        png_bytep row = rowPointers[i];
        for(unsigned int j = 0 ; j < givenWidth; j++) 
        {        
            png_bytep px = &(row[j * 3]);
            px[0] = static_cast<png_byte>(givenMatrix[i][j].r);
            px[1] = static_cast<png_byte>(givenMatrix[i][j].g);
            px[2] = static_cast<png_byte>(givenMatrix[i][j].b);
        }
    }

    png_set_rows (png_ptr, info_ptr, rowPointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png_ptr, NULL); 
    free(rowPointers);
}   

Pixel** Image::getRGBMatrix()
{
    return matrix;
}

Image::~Image()
{
    free(matrix);
}