#pragma once

#include <iostream>
#include <fstream>

//Buffer size in bits
#define BS_BUFFER_SIZE 8

enum bs_mode{read, write};

class bitStream {
    private:
        std::fstream fileStream;
        bs_mode mode;
        char buffer;
        int bufferCount;
        bool isOpen;
        
        bool bufferIsEmpty();
        bool bufferIsFull();
        bool handleReadError();
        bool handleWriteError();
        bool handleOpenError();
        bool handleCloseError();

    public:
        bitStream(const std::string file, bs_mode mode);
        ~bitStream();
        bool readBit(unsigned char &res);
        bool writeBit(const unsigned char bit);
        bool readNBits(unsigned char *bits, const unsigned int nBits);
        bool writeNBits(const unsigned char *bits, const unsigned int nBits);
        bool open(std::string file, bs_mode mode);
        bool close();
};