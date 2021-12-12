#include <string>
#include "BitStream.h"

class Golomb {
    public:
        Golomb(const std::string filePath, const BitStream::bs_mode mode, uint32_t m);
        void setM(uint32_t m);
        void encodeNumber(const int number);
        int32_t decodeNumber();
        bool fillWithPadding(uint8_t bit);
        bool fillWithPadding();
        
        bool close();
        bool open(std::string filePath, BitStream::bs_mode mode);
        bool isOpen();
    private:
        uint32_t m;
        BitStream stream;
        uint32_t foldNumber(const int32_t number);
        int32_t unfoldNumber(const uint32_t number);
};

