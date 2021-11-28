#pragma once

#include <iostream>
#include <fstream>

//Buffer size in bits
/** 
 * \brief Class to read and write bits from a file 
*/

class BitStream {
    public:
        /** 
         * \brief file mode
        */
        enum bs_mode{read, write}; 

        /**
         * @brief Construct a new BitStream object.
         * @param file Path to the target file.
         * @param mode 
         */
        BitStream(const std::string file, BitStream::bs_mode mode);
        
        /**
         * @brief Destroy the BitStream object
         * 
         */
        ~BitStream();

        /**
        * \brief Read a bit from BitStream.
        * 
        * Bit is writen in lsb, all remaining bits become 0.
        * \param res Reference where the read value is written to.
        */
        bool readBit(unsigned char &res);
        
        /**
        * \brief Write a bit to BitStream.
        * \param bit Bit value to be written (bit is assumed to be the lsb).
        */
        bool writeBit(const unsigned char bit);

        /**
        * \brief Read N bits from BitStream.
        * \param bit Array containing read bits.
        * \param nBits Number of bits to be read.
        */
        bool readNBits(unsigned char *bits, const unsigned int nBits);

        /**
        * \brief Write N bits from BitStream.
        * 
        * Bits are stored in a little endian fashion
        * \param bit Array containing bits to be writen.
        * \param nBits Number of bits to write.
        */
        bool writeNBits(const unsigned char *bits, const unsigned int nBits);

        /**
         * \brief Open BitStream to the file.
         * \param filePath Path to the target file.
         * \param mode
         * \sa BitStream::bs_mode.
         */
        bool open(std::string filePath, BitStream::bs_mode mode);

        /**
         * @brief Close BitStream.
         * 
         */
        bool close();

    private:
        #define bit_mask_to_read(nBits) ( (unsigned char) (std::pow(2, nBits) - 1) << (BUFFER_SIZE - nBits) )
        const u_char BUFFER_SIZE = 8;
        const u_char ALL_BITS_MASK = 0xFF; 
        std::fstream fileStream;
        bs_mode mode;
        char buffer;
        int bufferCount;
        bool isOpen;

        const bool bufferIsEmpty();
        const bool bufferIsFull();
        const bool handleReadError();
        const bool handleWriteError();
        const bool handleOpenError();
        const bool handleCloseError();    
};