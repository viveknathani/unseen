#include <iostream>
#include "../lib/Image.hpp"

/*
    argv[] has to be an array of length 4
    arg[0] = name of object file
    arg[1] = path to input image
    arg[2] = path to output image
    arg[3] = format (jpg/png) (0/1)
    
    arg[0]'s existence is irrelevant to the main function.
    Hence it is not extracted.
*/

int main(int argc, char** argv)
{
    // extracting the arguments
    std::string input = argv[1];
    std::string output = argv[2];
    std::string format = argv[3];
    int formatInt = std::stoi(format);
    
    //format validation
    if(!(formatInt == 0 || formatInt == 1))
    {
        std::cout << "Invalid file format. Expected 0 or 1." << std::endl;
        return 1;
    }

    Image img(input);
    Image newUsingImg(formatInt, img.getWidth(), img.getHeight(), img.getRGBMatrix(), output);
    return 0;
}