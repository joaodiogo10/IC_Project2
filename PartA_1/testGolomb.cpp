#include "Golomb.h"


std::string testFile = "testGolomb.txt";

void testEncoder();

int main(int argc, char *argv[])
{
    std::cout << "-------Test encoder-------" << std::endl;
    std::cout << "Writing numbers: 20, -15, -7, 2" << std::endl;
    // m = 7 
    // Reaminder representation
    // 00 0     0
    // 00 1     1
    // 01 0     2
    // 01 1     3
    // 10 0     4
    // 10 1     5
    // 11       6
    Golomb encoder(testFile, BitStream::bs_mode::write, 9);
    
    
    // folding: 20 -> 40
    // q = 40/7 = 5 (0000 01)
    // r = 5 (101)
    // 0000 0110 . 1 
    encoder.encodeNumber(20);

    // folding: -15 -> 29
    // q = 29/7 = 4 (0000 1)
    // r = 1 (001)
    // 0000 1001
    //
    // stream:   0000 0110 . 1000 0100 . 1
    encoder.encodeNumber(-15);

    // folding:  -7 -> 13
    // q = 13/7 = 1 (01)
    // r = 6 (11)
    // 0111
    //
    // stream:   0000 0110 . 1000 0100 . 1011 1
    encoder.encodeNumber(-7);

    // m = 4
    // Reaminder representation
    // 00   0
    // 01   1
    // 10   2
    // 11   3
    encoder.setM(4);

    // folding: 2 -> 4
    // q = 4/4 = 1 (01)
    // r = 0 (00)
    // 0100
    //
    // stream:   0000 0110 . 1000 0100 . 1011 1010 . 0
    encoder.encodeNumber(2);

    //file result: 0000 0110 . 1000 0100 . 1011 1010 . 0000 0000(06 84 BA 00)    
    encoder.close();

    std::cout << "-------Test decoder-------" << std::endl;

    Golomb decoder(testFile, BitStream::bs_mode::read, 9);
    int32_t number;

    number = decoder.decodeNumber();
    std::cout << "Decoded number: " << number << std::endl;
    
    number = decoder.decodeNumber();
    std::cout << "Decoded number: " << number << std::endl;
    
    number = decoder.decodeNumber();
    std::cout << "Decoded number: " << number << std::endl;
    
    decoder.setM(4);
    number = decoder.decodeNumber();
    std::cout << "Decoded number: " << number << std::endl; 
    
    return 0;
}