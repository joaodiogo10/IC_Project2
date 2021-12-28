#include "BitStream.h"

/** \file 
 *  Run test for class BitStream. Results are printed to console.
*/

/**
 * \brief Test \ref BitStream::writeBit 
 */
void testWriteBit(char c, BitStream &oStream);

/**
 * \brief Test \ref BitStream::readBit and BitStream::readNBits.
 * 
 * Results are printed to the console. Printed character or decimal values should match, if not something went wrong.\n
 * 
 * <pre>
 * Ex:\n
 * #: #     - read expected value\n 
 * #: c     - read don't match expected value\n 
 * </pre>
 */
void test1();
/**
 * \brief Test \ref BitStream::writeNBits and BitStream::readNBits
 * 
 * Results are printed to the console. Printed character or decimal values should match, if not something went wrong.\n
 * 
 * <pre>
 * Ex:\n
 * #: #     - read expected value\n 
 * #: c     - read don't match expected value\n 
 * </pre>
 */
void test2();

std::string test1File = "testBitStream1.txt"; /*!< first test file */
std::string test2File = "testBitStream2.txt"; /*!< second test file */

int main(int argc, char *argv[])
{
    //----------Test 1---------
    //-------------------------
    BitStream oStream(test1File, BitStream::bs_mode::write);
    // 0010 0011 #
    testWriteBit('#', oStream);
    // 0110 0011 c
    testWriteBit('c', oStream);
    // 0010 0010 "
    testWriteBit('"', oStream);
    // 0110 0011 c
    testWriteBit('c', oStream);

    oStream.close();

    test1();

    //----------Test 2---------
    //-------------------------
    test2();
   
    return 0;
}

void testWriteBit(char c, BitStream &oStream) {
    std::cout << "-------Test WriteBit -------" << std::endl;
    oStream.writeBit((c & 0x80) >> 7);
    oStream.writeBit((c & 0x40) >> 6);
    oStream.writeBit((c & 0x20) >> 5);
    oStream.writeBit((c & 0x10) >> 4);
    oStream.writeBit((c & 0x08) >> 3);
    oStream.writeBit((c & 0x04) >> 2);
    oStream.writeBit((c & 0x02) >> 1);
    oStream.writeBit(c & 0x01);
}

void test1() {
    std::cout << "-------Test 1-------" << std::endl;
    // file content
    // 0010 0011 #
    // 0110 0011 c
    // 0010 0010 "
    // 0110 0011 c
    BitStream iStream(test1File, BitStream::bs_mode::read);
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
    iStream.open(test1File, BitStream::bs_mode::read);

    //Read 2 bytes result should be input2[0] = 0010 0011 (#); input2[1] = 0110 0011 (c)
    iStream.readNBits(input2, 16);

    std::cout << "#c: ";
    std::cout << input2[0] << input2[1] << std::endl;
    iStream.close();

    iStream.open(test1File, BitStream::bs_mode::read);

    //Read 14 bits result should be input2[0] = 0010 0011 (#); input2[1] = 0001 1000 (24 decimal)
    iStream.readNBits(input2, 14);

    std::cout << "#24: ";
    std::cout << input2[0] << (u_int) input2[1] << std::endl;

    //Read 2 bits result should be input2[0] = 0000 0011 (3 decimal)
    iStream.readNBits(input2, 2);
    
    std::cout << "3: ";
    std::cout << (u_int)input2[0] << std::endl;
    iStream.close();

    iStream.open(test1File, BitStream::bs_mode::read);

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

    iStream.open(test1File, BitStream::bs_mode::read);

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

    //Read 12 bits result should be input2[0] = 0010 0110 (38 decimal)
    std::cout << "38: ";
    iStream.readNBits(input2, 12);
    std::cout << (u_int)input2[0] << std::endl;

    iStream.close();
}

void test2() {
    std::cout << "-------Test 2-------" << std::endl;
    BitStream oStream(test2File, BitStream::bs_mode::write);

    // 0001 0100    20 
    // 0001         01
    // 0001 1001    25
    // 0000 0111    07
    // 0111 0111    119

    unsigned char number = 20;

    oStream.writeNBits(&number, 8);

    number = 25;

    oStream.writeNBits(&number, 4);
    oStream.writeNBits(&number, 8);

    unsigned char numbers[2] = {7 , 119};
    oStream.writeNBits(numbers, 16);

    oStream.close();

    BitStream iStream(test2File, BitStream::bs_mode::read);

    u_char input;
    //Read 8 bits, result should be  input = 0001 0100 (20 decimal)
    iStream.readNBits(&input, 8);

    std::cout << "20: " << (u_int)input << std::endl;

    //Read 4 bits, result should be  input = 0001 (1 decimal)
    iStream.readNBits(&input, 4);

    std::cout << "1: " << (u_int)input << std::endl;

    //Read 8 bits, result should be  input = 0001 1001 (25 decimal)
    iStream.readNBits(&input, 8);

    std::cout << "25: " << (u_int)input << std::endl;


    unsigned char input2[2];
    //Read 16 bits, result should be  input2[0] = 0000 0111  (7 decimal); input2[1] = 0111 0111 ( 119 decimal )
    iStream.readNBits(input2, 16);

    std::cout << "7: " << (u_int)input2[0] << std::endl;
    std::cout << "119: " << (u_int)input2[1] << std::endl;

}

