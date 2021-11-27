#include "BitStream.h"

int main(int argc, char *argv[])
{

    BitStream oStream("test.txt", BitStream::bs_mode::write);

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

    // 0010 0010
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(0);

    // 0110 0011 c
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(1);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(0);
    oStream.writeBit(1);
    oStream.writeBit(1);

    //0001 0100
    unsigned char number = 20;

    oStream.writeNBits(&number, 8);

    number = 25;
    //0001 1001
    oStream.writeNBits(&number, 4);
    oStream.writeNBits(&number, 8);

    oStream.close();

    BitStream iStream("test.txt", BitStream::bs_mode::read);
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
    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 2 bytes result should be input2[0] = 0010 0011 (#); input2[1] = 0110 0011 (#)
    iStream.readNBits(input2, 16);

    std::cout << input2[0] << input2[1] << std::endl;
    iStream.close();

    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 14 bits result should be input2[0] = 0010 0011 (#); input2[1] = 0110 0000 (c)
    iStream.readNBits(input2, 14);

    std::cout << input2[0] << input2[1] << std::endl;

    //Read 2 bits result should be input2[0] = 1100 000 (192 decimal)
    iStream.readNBits(input2, 2);

    std::cout << (u_int)input2[0] << std::endl;
    iStream.close();

    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 4 bits result should be input2[0] = 0010 0000 (32 decimal)
    iStream.readNBits(input2, 4);

    std::cout << (u_int)input2[0] << std::endl;

    //Read 1 byte result should be input2[0] = 0011 0110 (54 decimal)
    iStream.readNBits(input2, 8);
    std::cout << (u_int)input2[0] << std::endl;

    //Read 12 bits result should be input2[0] = 0011 0010 (50 decimal); input2[1] = 0010 0000 (32 decimal)
    iStream.readNBits(input2, 12);
    std::cout << (u_int)input2[0] << "\n"
              << (u_int)input2[1] << std::endl;

    //Read 1 byte result should be input2[0] = 0110 0011 c
    iStream.readNBits(input2, 8);
    std::cout << input2[0] << std::endl;

    iStream.close();

    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 2 bits result should be input2[0] = 0000 0000 (0 decimal)
    iStream.readNBits(input2, 2);

    std::cout << (u_int)input2[0] << std::endl;

    //Read 4 bits result should be input2[0] = 1000 0000 (128 decimal)
    iStream.readNBits(input2, 4);
    std::cout << (u_int)input2[0] << std::endl;

    //Read 1 bit result should be input2[0] = 1000 000 (128 decimal)
    iStream.readNBits(input2, 1);
    std::cout << (u_int)input2[0] << "\n"
              << std::endl;

    //Read 1 byte result should be input2[0] = 1011 0001 (177 decimal)
    iStream.readNBits(input2, 8);
    std::cout << (u_int)input2[0] << std::endl;

    //Read 5 bits result should be input2[0] = 1001 0000 (144 decimal)
    iStream.readNBits(input2, 5);
    std::cout << (u_int)input2[0] << std::endl;

    iStream.readNBits(input2, 12);
    std::cout << (u_int)input2[0] << std::endl;

    //Read 8 bits, result should be  input2[0] = 0001 0100 (20 decimal)
    std::cout << "\n"
              << "Testing results of writing N bits" << std::endl;
    iStream.readNBits(input2, 8);
    std::cout << (u_int)input2[0] << std::endl;

    //Read 4 bits, result should be  input2[0] = 0001 (1 decimal)
    iStream.readNBits(input2, 4);
    std::cout << "\n"
              << (u_int)input2[0] << std::endl;

    return 0;
}
// file content
// 0010 0011 #
// 0110 0011 c
// 0010 0010
// 0110 0011 c
// 0001 0100
// 0001
// 0001 1001