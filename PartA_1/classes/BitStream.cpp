#include "BitStream.h"
#include <math.h>

BitStream::BitStream(const std::string file, BitStream::bs_mode mode)
{
    if (mode == BitStream::bs_mode::read)
        fileStream.open(file, std::ios_base::in | std::ios_base::binary);
    else
        fileStream.open(file, std::ios_base::out | std::ios_base::binary);

    if (handleOpenError()) {
        fOpen = false;
        return;
    }

    fOpen = true;
    this->buffer = 0;
    this->bufferCount = 0;
    this->mode = mode;
}

BitStream::~BitStream()
{
    if(fOpen && mode == bs_mode::write)
    {
        flushBuffer();
    }
}

const bool BitStream::bufferIsEmpty()
{
    return bufferCount == 0;
}

const bool BitStream::bufferIsFull()
{
    return bufferCount == 8;
}

bool BitStream::readBit(unsigned char &res)
{
    if (!validReadOperation())
        return false;

    //Fill buffer if needed
    if (bufferIsEmpty())
    {
        fileStream.read(&buffer, 1);
        if (handleReadError())
            return false;

        bufferCount = 8;
    }

    res = (buffer & ((int)std::pow(2, BUFFER_SIZE - 1))) >> 7;
    buffer = buffer << 1;
    bufferCount--;

    return true;
}

bool BitStream::writeBit(const unsigned char bit)
{
    if (!validWriteOperation())
        return false;
    
    buffer = (buffer << 1) | (bit & 0x01);
    bufferCount++;

    //flush buffer if needed
    if (bufferIsFull())
    {
        fileStream.write(&buffer, 1);

        if (handleWriteError())
            return false;

        bufferCount = 0;
    }

    return true;
}

bool BitStream::readNBits(unsigned char *bits, const unsigned int nBits)
{
    if(!validReadOperation())
        return false;

    int bitsToRead = nBits;

    //No bits to read
    if (nBits == 0)
        return true;

    //All bits are in buffer
    if (bufferCount >= bitsToRead)
    {
        bits[0] = (buffer & bit_mask_to_read(bitsToRead)) >> (BUFFER_SIZE - bitsToRead);
        buffer = buffer << bitsToRead;
        bufferCount -= bitsToRead;
        return true;
    }

    //Read all bits from buffer
    bits[0] = buffer & bit_mask_to_read(bufferCount);

    buffer = buffer << bufferCount;
    bitsToRead = bitsToRead - bufferCount;
    int arrayOffset = bufferCount;

    //At this point buffer is empty, fill it
    fileStream.read(&buffer, 1);
    if (handleReadError())
        return false;

    //If we are reading less than a byte or a byte
    if (nBits <= BUFFER_SIZE)
    {
        bits[0] = bits[0] | ((buffer & bit_mask_to_read(bitsToRead)) >> arrayOffset);
        //align do the left
        u_char emptyBits = BUFFER_SIZE - nBits;
        bits[0] = bits[0] >> emptyBits;
        buffer = buffer << bitsToRead;
        bufferCount = BUFFER_SIZE - bitsToRead;

        return true;
    }

    //if we are reading more than a byte
    int bytesRemaining = bitsToRead / 8;
    int bitsRemaining = bitsToRead % 8;

    //for each byte remaining fill array
    for (int i = 0; i < bytesRemaining; i++)
    {
        //Fill empty bits from previous byte
        u_char tmp = BUFFER_SIZE - arrayOffset;
        bits[i] = bits[i] | ((buffer & bit_mask_to_read(tmp)) >> arrayOffset);
        buffer = buffer << tmp;

        //Fill new byte with remaning bits in buffer
        bits[i + 1] = (buffer & bit_mask_to_read(arrayOffset));
        buffer = buffer << arrayOffset;

        if (i != bytesRemaining - 1)
        {
            fileStream.read(&buffer, 1);
            if (handleReadError())
                return false;
        }
    }

    if (bitsRemaining == 0)
    {
        //align last array byte do the left
        u_char emptyBits = BUFFER_SIZE - arrayOffset;
        bits[bytesRemaining] = bits[bytesRemaining] >> emptyBits;  
        bufferCount = 0;
        return true;
    }

    //fill array with bits remaining

    //read to buffer
    fileStream.read(&buffer, 1);
    if (handleReadError())
        return false;

    //if bits remaning is less or equal the space left in last byte of filled array
    if (bitsRemaining <= (BUFFER_SIZE - arrayOffset))
    {
        bits[bytesRemaining] = bits[bytesRemaining] | ((buffer & bit_mask_to_read(bitsRemaining)) >> arrayOffset);
        buffer = buffer << bitsRemaining;

        //align last array byte do the left
        u_char emptyBits = BUFFER_SIZE - (arrayOffset + bitsRemaining);
        bits[bytesRemaining] = bits[bytesRemaining] >> emptyBits; 
    }
    //else we will need another byte in array
    else
    {
        u_char tmp = BUFFER_SIZE - arrayOffset;
        bits[bytesRemaining] = bits[bytesRemaining] | ((buffer & bit_mask_to_read(tmp)) >> arrayOffset);
        buffer = buffer << tmp;

        tmp = bitsRemaining - (BUFFER_SIZE - arrayOffset);
        bits[bytesRemaining + 1] = buffer & bit_mask_to_read(tmp);
        buffer = buffer << tmp;

        //align last array byte do the left
        u_char emptyBits = tmp;
        bits[bytesRemaining] = bits[bytesRemaining] >> emptyBits; 
    }

    bufferCount = BUFFER_SIZE - bitsRemaining;

    return true;
}

bool BitStream::writeNBits(const unsigned char *bits, const unsigned int nBits)
{
    if (nBits == 0) {
        std::cout << "ERROR: Write more than 0 bits" << std::endl;
        return false;
    }
    if(!validWriteOperation())
        return false;

    int bytesToRead = double(nBits) / 8;
    int bitsRemaining = nBits % 8;
    unsigned char auxBuffer, bit;

    //For multiples of 8
    if (bitsRemaining == 0)
    {
        for (int i = 0; i < bytesToRead; i++)
        {
            auxBuffer = bits[i];

            for (int j = 0; j < 8; j++)
            {
                bit = auxBuffer & 0x80;
                bit = bit >> 7;
                auxBuffer = auxBuffer << 1;

                writeBit(bit);
            }
        }
    }
    //for the bits remaining
    else
    {
        for (int i = 0; i < bytesToRead; i++)
        {
            auxBuffer = bits[i];

            for (int j = 0; j < 8; j++)
            {
                bit = auxBuffer & 0x80;
                bit = bit >> 7;
                auxBuffer = auxBuffer << 1;
                writeBit(bit);
            }
        }

        auxBuffer = bits[bytesToRead];

        for (int j = 0; j < bitsRemaining; j++)
        {
            bit = auxBuffer & 0x80;
            bit = bit >> 7;
            auxBuffer = auxBuffer << 1;
            writeBit(bit);
        }
    }
    return true;
}

bool BitStream::flushBuffer(uint8_t bit) {
    if (!validWriteOperation())
        return false;

    if(bufferCount == 0)
        return true;
    
    uint8_t shiftAmount = (BUFFER_SIZE - bufferCount);

    char tmp = buffer << shiftAmount;
    if((bit & 0x01) == 1)
        tmp = tmp | ((uint8_t) std::pow(2,shiftAmount) * (bit & 0x01) - 1);
    
    fileStream.write(&tmp, 1);
    if (handleWriteError())
        return false;

    buffer = 0;
    bufferCount = 0;
    return true;
}

bool BitStream::flushBuffer() {
    return flushBuffer(0);
}

const bool BitStream::validReadOperation() {
    if (!fOpen)
    {
        std::cout << "ERROR: File is closed" << std::endl;
        return false;
    }

    if (mode != BitStream::bs_mode::read)
    {
        std::cout << "ERROR: Can read from output file" << std::endl;
        return false;
    }
    return true;
}

const bool BitStream::validWriteOperation() {
    if (!fOpen)
    {
        std::cout << "ERROR: File is closed" << std::endl;
        return false;
    }
    if (mode != BitStream::bs_mode::write)
    {
        std::cout << "ERROR: Can write from input file" << std::endl;
        return false;
    }

    return true;
}

bool BitStream::open(std::string file, BitStream::bs_mode mode) {
    if (fOpen)
    {
        std::cerr << "ERROR: File is already opened!" << std::endl;
        return false;
    }

    if (mode == BitStream::bs_mode::read)
        fileStream.open(file, std::ios_base::in | std::ios_base::binary);
    else
        fileStream.open(file, std::ios_base::out | std::ios_base::binary);

    if (handleOpenError())
        return false;

    fOpen = true;
    this->buffer = 0;
    this->bufferCount = 0;
    this->mode = mode;
    return true;
}

bool BitStream::close() {
    if (!fOpen)
    {
        std::cerr << "ERROR: File is already closed!" << std::endl;
        return false;
    }
    if(mode == BitStream::bs_mode::write)
        flushBuffer();

    fileStream.close();

    if (handleCloseError())
        return false;

    fOpen = false;
    return true;
}

const bool BitStream::isOpen() {
    return fOpen;
}

const bool BitStream::handleReadError()
{
    if (fileStream.eof())
    {
        std::cerr << "ERROR: End of file reached" << std::endl;
        return true;
    }

    if (fileStream.fail())
    {
        std::cerr << "ERROR: Failed to read the file" << std::endl;
        return true;
    }

    return false;
}

const bool BitStream::handleWriteError()
{
    if (fileStream.fail())
    {
        std::cerr << "ERROR: Failed to write the file" << std::endl;
        return true;
    }

    return false;
}

const bool BitStream::handleOpenError()
{
    if (!fileStream.is_open())
    {
        std::cerr << "ERROR: Failed to open file" << std::endl;
        return true;
    }
    return false;
}

const bool BitStream::handleCloseError()
{
    if (fileStream.is_open())
    {
        std::cerr << "ERROR: Unable to close file" << std::endl;
        return false;
    }
    return false;
}