#include "Golomb.h"

/** \file 
 *  Run test for class Golomb. Results are printed to console.
*/

std::string testFile1 = "testGolomb1.txt"; /*!< first test file */
std::string testFile2 = "testGolomb2.txt"; /*!< second test file */

/**
 * \brief Golomb test 1
 * 
 * Manual test for M = 7, and M = 4. Numbers 20,-15,-7 and 2 are encoded to file testGolomb1.txt and then decoded.\n
 * Decoded number should be the same as encoded numbers, i.e., 20, -15, -7 and 2.
 */
void test1();

/**
 * \brief Golomb test 2
 * 
 * Tests golomb numeric codec. In this test all integer values between -1000 and 1000 are encoded to testGolomb2.txt and then decoded.\n
 * Decoded numbers should be the same as encoded numbers. If not, return false.
 * 
 * \param m M parameter of golomb code.
 * \return true, if test succeded.
 * \return false, if test failed.
 */
bool test2(uint16_t m);

int main(int argc, char *argv[])
{

    test1();

    bool testPassed = true;
    //test m = 1 to m = 500
    for(int i = 1; i <= 500; i++ ) {
        if(!test2(i))
        {
            std::cout << "Test failed for M = " << i << "!!!\n" << std::endl;
            testPassed = false;
        }
    }

    if(testPassed)
        std::cout << "All tests succeeded!!" << std::endl;

    return 0;
}

void test1() {
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
    Golomb encoder(testFile1, BitStream::bs_mode::write, 7);
    
    
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

    Golomb decoder(testFile1, BitStream::bs_mode::read, 7);
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
    
    decoder.close();
}

bool test2(uint16_t m) {

    Golomb encoder(testFile2, BitStream::bs_mode::write, m);
    bool success = true;

    for(int i = -1000; i < 1000; i++) {
        encoder.encodeNumber(i);
    }
    encoder.close();

    Golomb decoder(testFile2, BitStream::bs_mode::read, m);
    int decodedNum;

    for(int i = -1000; i < 1000; i++) {
        decodedNum = decoder.decodeNumber();
        if(decodedNum != i) {
            success = false;
            break;
        }
    }
    decoder.close();

    return success; 
}