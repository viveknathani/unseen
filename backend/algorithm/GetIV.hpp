#include <algorithm>
#include <string>
#include <time.h>

std::string getIV()
{
    std::string result;
    char possibleValues[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    srand(time(NULL));
    for(int i = 0; i < 32; i++)
        result += possibleValues[rand() % 16];
    
    std::random_shuffle(result.begin(), result.end());
    return result;
}