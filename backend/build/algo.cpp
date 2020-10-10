#include <iostream>
#include "../algorithm/AES256.hpp"

/*
    argv[] has to be an array of length 5
    arg[0] = name of object file
    arg[1] = input hex string
    arg[2] = key hex string
    arg[3] = IV hex string
    arg[4] = encrypt/decrypt (0/1)

    Encryption has two possibilities.
    case(i) : The user has a IV.
    case(ii) : The user does not have an IV.

    if(case(i)) 
        the last argument of the constructor is false
    else 
        the arg[3] has to be "00" and constructor has to pass true.
        the program will output the IV in this case.
        it is the responsibility of the user to collect that IV.
    
    arg[0]'s existence is irrelevant in our case.
    Hence it is not extracted.
*/

int main(int argc, char** argv)
{
    std::string input = argv[1];
    std::string key = argv[2];
    std::string IV = argv[3];
    std::string task = argv[4];
    int formatTask = std::stoi(task);

    if(!(formatTask == 0 || formatTask == 1))
    {
        std::cout << "Invalid task format (fifth argument)." << std::endl;
        return 1;
    }

    if((int)key.length() != 64)
    {
        std::cout << "Invalid key provided." << std::endl;
        return 1;
    }

    if(formatTask == 1 && (int)IV.length() != 32)
    {
        std::cout << "Invalid IV provided." << std::endl;
        return 1;
    }

    bool lastArgument = (IV == "00");

    AES256 object(input, key, IV, formatTask, lastArgument);
    std::cout << object.getHexOutput() << std::endl;
    if(lastArgument) std::cout << IV << std::endl;
    return 0;
}