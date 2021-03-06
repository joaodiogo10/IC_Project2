#include <string>
#include <vector>
#include "BitStream.h"

/** \file 
 *  Golomb class header file.
*/

/** 
 * \brief Class that codes, to a file, or encodes, from a file, golomb codes.
 *
 * This class uses \ref BitStream in order to read bits from a file or write bits to a file.
 * All errors are handled by \ref BitStream class. Methods from this class simply return false, 
 * propagating an error condition, in case an error occurs in a \ref BitStream operation.
 * \see BitStream
*/
class Golomb {
    public:
        /**
         * \brief Construct a new Golomb object.
         * \param[in] filePath Path to the target file.
         * \param[in] mode 
         * \param[in] m M parameter of Golomb code.
         * \sa BitStream::bs_mode
         */
        Golomb(const std::string filePath, const BitStream::bs_mode mode, const uint32_t m);

        /**
         * \brief Sets M parameter of Golomb code.
         * 
         * \param[in] m 
         */
        void setM(const uint32_t m);

        /**
         * \brief Encode a number and write it to the \ref BitStream
         * 
         * \param[in] number 
         */
        void encodeNumber(const int number);
        
        /**
         * \brief Decode a number from the \ref BitStream
         * 
         * \param number 
         * \return Decoded number.
         */
        int32_t decodeNumber();

        /**
         * \brief Close \ref BitStream.
         * 
         * \return true, if no error occured.
         * \return false, if an error occured.
         */
        bool close();
        
        /**
         * \brief Open \ref BitStream to a target file.
         * 
         * \param[in] filePath path to the target file
         * \param[in] mode 
         * \return true, if no error occured.
         * \return false, if an error occured.
         * 
         * \sa BitStream::bs_mode
         */
        bool open(const std::string filePath, const BitStream::bs_mode mode);

        /**
         * \brief Verify if \ref BitStream is open.
         * 
         * \return true, if \ref BitStream is open.
         * \return false, if \ref BitStream is closed.
         */
        bool isOpen();

        /**
         * \brief Get optimal M for a given distribution of values
         * 
         * \param[in] values vector contains all values
         * \return uint optimal M
         */
        static uint getOtimizedM(const std::vector<int> values);
        
    private:
        uint32_t m;
        BitStream stream;
        static uint32_t foldNumber(const int32_t number);
        static int32_t unfoldNumber(const uint32_t number);
};

