#include "Golomb.h"


std::string testFile = "testGolomb.txt";


int main(int argc, char *argv[])
{
    // m = 7
    // Reaminder representation
    // 00 0     0
    // 00 1     1
    // 01 0     2
    // 01 1     3
    // 10 0     4
    // 10 1     5
    // 11       6

    Golomb encoder(testFile, BitStream::bs_mode::write, 7);
    
    int32_t number = 20;
    
    // folding: 20 -> 40
    // q = 40/7 = 5
    // r = 5
    // 0000 0110 1  
    encoder.encodeNumber(20);

    return 0;
}
