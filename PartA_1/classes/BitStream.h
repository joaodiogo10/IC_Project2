#pragma once

#include <iostream>
#include <fstream>

/** 
 * \brief Class to read and write bits from a file. 
 * 
 * All errors are handled by this class, simply printing to standard output an error or warning massage.
 * Methods that can cause an error to occur return false when it occurs.
*/
class BitStream
{
public:
    /** 
         * \brief file mode
        */
    enum bs_mode
    {
        read,
        write
    };

    /**
         * \brief Construct a new BitStream object.
         * \param file Path to the target file.
         * \param mode
         * \sa BitStream::bs_mode. 
         */
    BitStream(const std::string file, BitStream::bs_mode mode);

    /**
         * @brief Destroy the BitStream object
         * 
         * If stream buffer isn't empty, the remaining bits in the buffer are flushed to the stream.
         * \sa flushBuffer
         */
    ~BitStream();

    /**
        * \brief Read a bit from BitStream.
        * 
        * Bit is writen in LSB, all remaining bits become 0.
        * 
        * Ex:
        * 0000 0001 (bit 1).
        * 0000 0000 (bit 0).
        * \param res Reference where the read value is written to.
        * \return True, if no errors occurred.
        * \return False, if errors occurred.
        */
    bool readBit(unsigned char &res);

    /**
        * \brief Write a bit to BitStream.
        * \param bit Bit value to be written (bit is assumed to be in the LSB).
        */
    bool writeBit(const unsigned char bit);

    /**
        * \brief Read N bits from BitStream.
        * 
        * If \p nBits < 8, the reaminder bits are written to the first \"\p nBits % 8\" LSBs.
        * 
        * Ex: \n Next bits of file stream = 1010 0000 1010 1111 \n nBits = 12.
        * 
        * Result: \n
        * \p bits[0] = 1010 0000 \n
        * \p bits[1] = 0000 1010 \n
        * \param bit Unsigned char array containing read bits.
        * \param nBits Number of bits to be read.
        * \return True, if no errors occurred.
        * \return False, if errors occurred.
        */
    bool readNBits(unsigned char *bits, const unsigned int nBits);

    /**
        * \brief Write N bits from BitStream.
        * 
        * Writes \p nBits starting at the most significant bit.
        * 
        * \param bit Array containing bits to be writen.
        * \param nBits Number of bits to write.
        * \return True, if no errors occurred.
        * \return False, if errors occurred.
        */
    bool writeNBits(const unsigned char *bits, const unsigned int nBits);

    /**
         * \brief Open BitStream to the file.
         * \param filePath Path to the target file.
         * \param mode
         * \sa BitStream::bs_mode.
         * \return True, if no errors occurred.
         * \return False, if errors occurred.
         */
    bool open(std::string filePath, BitStream::bs_mode mode);

    /**
         * @brief Flush all bits in stream buffer to the stream
         * 
         * if \p bits & 0x01 == 0, 0's are introduced as padding to write an entire byte.
         * Otherwise, 1's are introduced. 
         * 
         * Ex:\n
         * \p bit = 0\n
         * Bits in buffer 101111. \n
         * Bits written 10111100.
         * 
         * \p bit = 1 \n
         * Bits in buffer 101111. \n
         * Bits written 10111111.
         *  
         * \return True, if no errors occurred.
         * \return False, if errors occurred.
         */
    bool flushBuffer(uint8_t bit);

    /**
         * @brief Flush all bits in stream buffer to the stream
         * 
         * Simply calls \ref flushBuffer(0)
         *  
         * \return True, if no errors occurred.
         * \return False, if errors occurred.
         */
    bool flushBuffer();

    /**
         * @brief Close BitStream.
         * 
         * \return True, if no errors occured
         * \return False, if errors occured
         */

    bool close();

    /**
         * @brief Open BitStream.
         * 
         * \return True, if stream is open
         * \return False, if stream is closed
         */
    const bool isOpen();

private:
#define bit_mask_to_read(nBits) ((unsigned char)(std::pow(2, nBits) - 1) << (BUFFER_SIZE - nBits))
    const u_char BUFFER_SIZE = 8;
    const u_char ALL_BITS_MASK = 0xFF;
    std::fstream fileStream;
    bs_mode mode;
    char buffer;
    int bufferCount;
    bool fOpen;

    const bool bufferIsEmpty();
    const bool bufferIsFull();

    const bool validWriteOperation();
    const bool validReadOperation();
    const bool handleReadError();
    const bool handleWriteError();
    const bool handleOpenError();
    const bool handleCloseError();
};