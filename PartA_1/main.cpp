#include "BitStream.h"

void testWriteBit(char c, BitStream &oStream);
void testReadNBits();
void testWriteNBits();

int main(int argc, char *argv[])
{
    BitStream oStream("test1.txt", BitStream::bs_mode::write);
    // 0010 0011 #
    testWriteBit('#', oStream);
    // 0110 0011 c
    testWriteBit('c', oStream);
    // 0010 0010 "
    testWriteBit('"', oStream);
    // 0110 0011 c
    testWriteBit('c', oStream);

    oStream.close();

    testReadNBits();

    testWriteNBits();
   
    return 0;
}

void testWriteBit(char c, BitStream &oStream) {
    oStream.writeBit((c & 0x80) >> 7);
    oStream.writeBit((c & 0x40) >> 6);
    oStream.writeBit((c & 0x20) >> 5);
    oStream.writeBit((c & 0x10) >> 4);
    oStream.writeBit((c & 0x08) >> 3);
    oStream.writeBit((c & 0x04) >> 2);
    oStream.writeBit((c & 0x02) >> 1);
    oStream.writeBit(c & 0x01);
}

void testReadNBits() {
    // file content
    // 0010 0011 #
    // 0110 0011 c
    // 0010 0010 "
    // 0110 0011 c
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

    //input = 0010 0011 #
    std::cout << "#: ";
    std::cout << input << std::endl;

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

    //input = 0010 0011 c
    std::cout << "c: ";
    std::cout << input << std::endl;
    iStream.close();

    unsigned char input2[2];
    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 2 bytes result should be input2[0] = 0010 0011 (#); input2[1] = 0110 0011 (c)
    iStream.readNBits(input2, 16);

    std::cout << "#c: ";
    std::cout << input2[0] << input2[1] << std::endl;
    iStream.close();

    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 14 bits result should be input2[0] = 0010 0011 (#); input2[1] = 0001 1000 (24 decimal)
    iStream.readNBits(input2, 14);

    std::cout << "#24: ";
    std::cout << input2[0] << (u_int) input2[1] << std::endl;

    //Read 2 bits result should be input2[0] = 0000 0011 (3 decimal)
    iStream.readNBits(input2, 2);
    
    std::cout << "3: ";
    std::cout << (u_int)input2[0] << std::endl;
    iStream.close();

    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 4 bits result should be input2[0] = 0000 0010 (2 decimal)
    iStream.readNBits(input2, 4);
    std::cout << "2: ";
    std::cout << (u_int)input2[0] << std::endl;

    //Read 1 byte result should be input2[0] = 0011 0110 (54 decimal)
    iStream.readNBits(input2, 8);
    std::cout << "54: ";
    std::cout << (u_int)input2[0] << std::endl;
    
    //Read 12 bits result should be input2[0] = 0011 0010 (50 decimal); input2[1] = 0000 0010 (2 decimal)
    iStream.readNBits(input2, 12);
    std::cout << "50: " << (u_int)input2[0] << std::endl
              << "2: " << (u_int)input2[1] << std::endl;

    //Read 1 byte result should be input2[0] = 0110 0011 c
    iStream.readNBits(input2, 8);
    std::cout << "c: ";
    std::cout << input2[0] << std::endl;

    iStream.close();

    iStream.open("test.txt", BitStream::bs_mode::read);

    //Read 2 bits result should be input2[0] = 0000 0000 (0 decimal)
    iStream.readNBits(input2, 2);

    std::cout << "0: ";
    std::cout << (u_int)input2[0] << std::endl;

    //Read 4 bits result should be input2[0] = 0000 1000 (8 decimal)
    iStream.readNBits(input2, 4);
    std::cout << "8: ";
    std::cout << (u_int)input2[0] << std::endl;

    //Read 1 bit result should be input2[0] = 0000 0001 (1 decimal)
    iStream.readNBits(input2, 1);
    std::cout << "1: ";
    std::cout << (u_int)input2[0] <<  std::endl;

    //Read 1 byte result should be input2[0] = 1011 0001 (177 decimal)
    iStream.readNBits(input2, 8);
    std::cout << "177: ";
    std::cout << (u_int)input2[0] << std::endl;

    //Read 5 bits result should be input2[0] = 0001 0010 (18 decimal)
    iStream.readNBits(input2, 5);
    std::cout << "18: ";
    std::cout << (u_int)input2[0] << std::endl;

    //Read 12 bits result should be input2[0] = 0010 0110 (32 decimal)
    std::cout << "32: ";
    iStream.readNBits(input2, 12);
    std::cout << (u_int)input2[0] << std::endl;

    iStream.close();
}

void testWriteNBits() {
    BitStream oStream("test2.txt", BitStream::bs_mode::write);

    // 0001 0100
    // 0001
    // 0001 1001
    
    // 1110 1111 1111
    // char[0] = 1110 1111
    // char[1] = 1111

    //0001 0100
    unsigned char number = 20;

    oStream.writeNBits(&number, 8);

    number = 25;
    //0001 1001
    oStream.writeNBits(&number, 4);
    oStream.writeNBits(&number, 8);

    oStream.close();

    BitStream iStream("test2.txt", BitStream::bs_mode::read);

    u_char input;
    //Read 8 bits, result should be  input2[0] = 0001 0100 (20 decimal)
    std::cout << "\n"
              << "Testing results of writing N bits" << std::endl;
    iStream.readNBits(&input, 8);
    std::cout << (u_int)input << std::endl;

    //Read 4 bits, result should be  input2[0] = 0001 (1 decimal)
    iStream.readNBits(&input, 4);
    std::cout << "\n"
              << (u_int)input << std::endl;

}

