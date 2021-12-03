#include "Golomb.h"
#include "math.h"

// TO DO: check stream
Golomb::Golomb(const std::string filePath, const BitStream::bs_mode mode, uint32_t m) : stream(filePath, mode){
    //check stream
    this->m = m;
}

void Golomb::setM(uint32_t m) {
    if(m = 0)
        printf("ERROR: m can't be 0");
    
    this->m = m;
}

void Golomb::encodeNumber(const int32_t number) {
    int32_t q, r;
    uint32_t wrNumber = foldNumber(number);
    q = wrNumber / m;
    r = wrNumber % m;

    //-----write unary code(q)-----
    for(uint32_t i = 0; i < q; i++)
        stream.writeBit(0);
    stream.writeBit(1);
    
    //if m == 1, there is no remainder
    if(m == 1) {
        return;
    }
    //-----write binary code(r)-----
    uint8_t nBitsMin = std::floor(log2(m));
    uint8_t nBitsMax = std::ceil(log2(m));

    //m is multiple of 2
    if(nBitsMax == nBitsMin) {
        uint8_t bitPos = (uint8_t) std::log2(r);
        uint8_t bytes = std::ceil( nBitsMax / (double) 8);

        do {
            int32_t mask = std::pow(2, bitPos);
            int8_t bit = (r & mask) >> bitPos;
            stream.writeBit(bit);
            bitPos--;
        }while(bitPos != 0);
        
        return;
    }

    //m isn't multiple of 2
    uint32_t threshold = (m - 2^nBitsMin) * 2;
    uint8_t nBits;

    if(r < threshold)
        nBits = nBitsMax;
    else
        nBits = nBitsMin;

    uint8_t bitPos = nBits;    
    do {
        int32_t mask = std::pow(2, bitPos);
        int8_t bit = (r & mask) >> bitPos;
        stream.writeBit(bit);
        bitPos--;
    }while(bitPos != 0);

    return;
}

int32_t Golomb::decodeNumber() {
    uint8_t bit;
    uint32_t q, r, number = 0;

    //-----read q-----
    stream.readBit(bit);
    while(bit != 0) { 
        stream.readBit(bit);
        q++;
    }

    //if m == 1, number == q
    if(m == 1) {
        return unfoldNumber(q);
    }

    //-----read r-----
    uint8_t nBitsMin = std::floor(log2(m));
    uint8_t nBitsMax = std::ceil(log2(m));
    
    uint8_t readBits = 0;
    while(readBits < nBitsMin) {
        stream.readBit(bit);
        r = r << 1;        
        r = r | bit;
    }

    uint32_t threshold = (m - 2^nBitsMin) * 2; 
    if((r << 1) < threshold) {
        stream.readBit(bit);
        r = r << 1;        
        r = r | bit;
    }

    number = q * m + r;
    return unfoldNumber(number);
}

uint32_t Golomb::foldNumber(const int32_t number) {
    if(number >= 0) 
        return 2 * number;
    else
        return 2 * std::abs(number) - 1;
}

int32_t Golomb::unfoldNumber(const uint32_t number) {
    if(number % 2 != 0) 
        return number / 2;
    else
        return - ((number + 1) / 2);
}