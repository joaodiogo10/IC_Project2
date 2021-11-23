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
    unsigned char tmp = 0;

    iStream.readBit(tmp);
    input = input | (tmp << 7);
    iStream.readBit(tmp);
    input = input | (tmp << 6);
    iStream.readBit(tmp);
    input = input | (tmp << 5);
    iStream.readBit(tmp);
    input = input | (tmp << 4);
    iStream.readBit(tmp);
    input = input | (tmp << 3);
    iStream.readBit(tmp);
    input = input | (tmp << 2);
    iStream.readBit(tmp);
    input = input | (tmp << 1);
    iStream.readBit(tmp);
    input = input | tmp;

    std::cout << input;

    input = 0;
    iStream.readBit(tmp);
    input = input | (tmp << 7);
    iStream.readBit(tmp);
    input = input | (tmp << 6);
    iStream.readBit(tmp);
    input = input | (tmp << 5);
    iStream.readBit(tmp);
    input = input | (tmp << 4);
    iStream.readBit(tmp);
    input = input | (tmp << 3);
    iStream.readBit(tmp);
    input = input | (tmp << 2);
    iStream.readBit(tmp);
    input = input | (tmp << 1);
    iStream.readBit(tmp);
    input = input | tmp;


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