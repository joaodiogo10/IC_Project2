#include "BitStream.h"

int main(int argc, char *argv[]) {

    bitStream oStream("test.txt", bs_mode::write);

    // 0010 0011 #
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(1);

    // 0110 0011 c
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(1);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(1);
    oStream.close();



    bitStream iStream("test.txt", bs_mode::read);
    unsigned char input = 0;
    iStream.readBit(input);
    
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    std::cout << input;

    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);
    input = input >> 1;
    iStream.readBit(input);

    std::cout << input << std::endl;
    iStream.close();


    unsigned char input2[2];
    iStream.open("test.txt", bs_mode::read);

    //Read 2 bytes result should be input2[0] = 0010 0011 (#); input2[1] = 0110 0011 (c)
    iStream.readNBits(input2, 16);
    
    std::cout << input2[0] << input2[1] << std::endl;
    iStream.close();



    iStream.open("test.txt", bs_mode::read);
    //Read 2 bytes result should be input2[0] = 0010 0011 (#); input2[1] = 0010 0011 (#)
    iStream.readNBits(input2, 14);
    
    std::cout << input2[0] << input2[1] << std::endl;
    iStream.close();

    return 0;
}