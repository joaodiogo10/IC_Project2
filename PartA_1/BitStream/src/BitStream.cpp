#include "BitStream.h"
#include <math.h>

bitStream::bitStream(const std::string file, bs_mode mode) {
    if(mode == bs_mode::read)
        fileStream.open(file, std::ios_base::in | std::ios_base::binary);
    else
        fileStream.open(file, std::ios_base::out | std::ios_base::binary);

    if(handleOpenError()) {
        isOpen = false;
        std::cerr << "ERROR: Failed to open file" << std::endl;
        return;
    }

    isOpen = true;
    this->buffer = 0;
    this->bufferCount = 0;
    this->mode = mode;
}

bitStream::~bitStream() {
    if(mode == bs_mode::write && bufferCount != 0)
        std::cerr << "WARNING: Some bits might not have been written" << std::endl;
}

bool bitStream::bufferIsEmpty() {
    return bufferCount == 0;
}

bool bitStream::bufferIsFull() {
    return bufferCount == 8;
}

bool bitStream::readBit(unsigned char &res) {   
    if(mode != bs_mode::read) {
        std::cout << "ERROR: Can read from output file" << std::endl;
        return false;
    }
    
    //Fill buffer if needed
    if(bufferIsEmpty()) {
        fileStream.read(&buffer, 1);
        if(handleReadError())
            return false;
        
        bufferCount = 8;
    }  

    res = (buffer & 128) >> 7;
    buffer = buffer << 1;
    bufferCount--;

    return true;
}

bool bitStream::writeBit(const unsigned char bit) {
    if(mode != bs_mode::write) {
        std::cout << "ERROR: Can write from input file" << std::endl;
        return false;
    }

    buffer = (buffer << 1) | ( bit & 0x01);
    bufferCount++;
    
    //flush buffer if needed
    if(bufferIsFull()) {
        fileStream.write(&buffer, 1);
            
        if(handleWriteError()) 
            return false;

        bufferCount = 0;
    } 

    return true; 
} 

void bitStream::readNBits(unsigned char *bits, const unsigned int nBits) {
    int bitsToRead = nBits;
    
    //No bits to read
    if(nBits == 0)
        return;

    //All bits are in buffer
    if(bufferCount >= bitsToRead) {
        bits[0] = buffer && (std::pow(2, bitsToRead) - 1);
        buffer = buffer >> bitsToRead;
        bufferCount -= bitsToRead;
        return;
    }

    //Read all bits from buffer
    bits[0] = buffer && (std::pow(2, bufferCount) - 1);
    bitsToRead = bitsToRead - bufferCount;
    int arrayOffset = bufferCount;

    //At this point buffer is empty, fill it
    fileStream.read(&buffer, 1);
    if(handleReadError())
        return;
    
    //If we are reading less then a byte or a byte
    if(nBits <= BS_BUFFER_SIZE) {
        bits[0] = (buffer & ((int) std::pow(2, bitsToRead) - 1)) << arrayOffset;
        buffer = buffer >> bitsToRead;
        bufferCount = BS_BUFFER_SIZE - bitsToRead;

        return;
    }

    //if we are reading more then a byte
    int bytesRemaining = bitsToRead / 8;
    int bitsRemaining = bitsToRead % 8;

    //for each byte remaining fill array
    for(int i = 0; i < bytesRemaining; i++) {
        //Fill empty bits from previous byte
        bits[i] = (buffer & (int) (std::pow(2, BS_BUFFER_SIZE - arrayOffset) - 1) ) << arrayOffset;

        //Fill new byte with reaming bits in buffer
        bits[i+1] = buffer & ((int) (std::pow(2, arrayOffset) -1) << (BS_BUFFER_SIZE - arrayOffset) );

        if(i != bytesRemaining - 1) {
            fileStream.read(&buffer,1);
            if(handleReadError())
                return;
        }
    }

    if(bitsRemaining == 0){
        bufferCount = 0;
        return;
    }

    //fill array with bits remaining

    //read to buffer
    fileStream.read(&buffer,1);
    if(handleReadError())
        return;

    //if bits remaning is less or equal the space left in last byte of filled array
    if(bitsRemaining <= BS_BUFFER_SIZE - arrayOffset) {
        bits[bytesRemaining] = (buffer & (int) (std::pow(2, bitsRemaining) - 1) ) << arrayOffset;
    }
    //else we will need another byte in array
    else {
        bits[bytesRemaining] = (buffer & (int) (std::pow(2, BS_BUFFER_SIZE - arrayOffset) - 1) ) << arrayOffset;
        bits[bytesRemaining + 1] = buffer & ((int) std::pow(2, bitsRemaining - (BS_BUFFER_SIZE - arrayOffset)) << (BS_BUFFER_SIZE - arrayOffset));
    }

    buffer = buffer >> bitsRemaining;
    bufferCount = BS_BUFFER_SIZE - bitsRemaining;

    return;
}

void bitStream::writeNBits(const unsigned char *bit, const unsigned int nBits) {
    
}

bool bitStream::open(std::string file, bs_mode mode) {
    if(isOpen) {
        std::cerr << "ERROR: File is already opened!" << std::endl;
        return false;
    }

    if(mode == bs_mode::read)
        fileStream.open(file, std::ios_base::in | std::ios_base::binary);
    else
        fileStream.open(file, std::ios_base::out | std::ios_base::binary);

    if(handleOpenError())
        return false;

    isOpen = true;
    this->buffer = 0;
    this->bufferCount = 0;
    this->mode = mode;
    return true;
}

bool bitStream::close() {
    if(!isOpen) {
        std::cerr << "ERROR: File is already closed!" << std::endl;
        return false;
    }
    fileStream.close();
    if(handleCloseError())
        return false;


    isOpen = false;
    return true;
}

bool bitStream::handleReadError() {
    if(fileStream.eof()) {
        std::cerr << "ERROR: End of file reached" << std::endl;
        return true;
    }

    if(fileStream.fail()) {
        std::cerr << "ERROR: Failed to read the file" << std::endl;
        return true;
    }

    return false;
}

bool bitStream::handleWriteError() {
    if(fileStream.fail()) {
        std::cerr << "ERROR: Failed to write the file" << std::endl;
        return true;
    }

    return false;
}

bool bitStream::handleOpenError() {
    if(!fileStream.is_open()) {
        isOpen = false;
        std::cerr << "ERROR: Failed to open file" << std::endl;
        return true;
    }
    return false;
}

bool bitStream::handleCloseError() {
    if(fileStream.is_open()) {
        std::cerr << "ERROR: Unable to close file" << std::endl;
        return false;
    }
    return false;
}