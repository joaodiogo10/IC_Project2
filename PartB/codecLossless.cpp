#include "../PartA_1/classes/Golomb.h"
#include "AudioFile/AudioFile.h"
#include <sstream>
#include <math.h>
#include <map>

/** \file 
 *  Lossless Codec for audio files.
*/

//------------------------------------------------------------
//-------------------Function prototypes----------------------
//------------------------------------------------------------
/**
 * \brief Calculate residuals entropy and generate matlab files
 * 
 * Channels' residuals are passed has a vector of vectors containing the residuals values of each channel.\n
 * The return vector containing the entropies result will match the order of residuals vector passed as argument.\n
 *
* Files are writting to matlab directory as channel1.txt and channel2.txt.\n
 * \param[in] residuals Vector of vectors contains residuals of each channel
 * \param[in] bitDepth bitDepth of the audio channel
 * \return Vector of integers from the converted audio channel
 */
std::vector<double> calculateEntropyAndResidualHistograms(const std::vector<std::vector<int>> &residuals, const int bitDepth);

/**
 * \brief Converts the amplitude of an audio channel form a vector of doubles to a vector of integers
 * 
 * \param[in] channel Vector of doubles from the audio channel to convert
 * \param[in] bitDepth bitDepth of the audio channel
 * \return Vector of integers from the converted audio channel
 */
std::vector<int> convertChannelAmplitudeToInteger(const std::vector<double> channel, const int bitDepth);

/**
 * \brief Converts the amplitude of an audio channel form a vector of integers to its original state
 * 
 * \param[in] channel Vector of integers from the converted audio channel
 * \param[in] bitDepth bitDepth of the audio channel
 * \return Vector of doubles from the original audio channel
 */
std::vector<double> revertChannelAmplitudeToDouble(const std::vector<int> channel, const int bitDepth);

/**
 * \brief Encodes file on a first order predictor basis
 * 
 * \param[in] audioFile audioFile to encode
 * \param[in] outputFilePath name of the encoded output file
 * \return Vector of the residual values of the encoded file
 */
std::vector<std::vector<int>> firstOrderPredictorEncoder(const AudioFile<double> audioFile, const std::string outputFilePath);

/**
 * \brief Decodes file on a first order predictor basis
 * 
 * \param[in] encodedFilePath path of the predictor encoded file
 * \param[in] outputFilePath name of the decoded output file
 * \return Decoded audio file
 */
void firstOrderPredictorDecoder(const std::string encodedFilePath, const std::string outputFilePath);

/**
 * \brief Encodes file on a polynomial predictor basis
 * 
 * \param[in] audioFile audioFile to encode
 * \param[in] outputFilePath name of the encoded output file
 * \return Vector of the residual values of the encoded file
 */
std::vector<std::vector<int>> polynomialPredictorEncoder(const AudioFile<double> audioFile, const  std::string outputFilePath);

/**
 * \brief Decodes file on a polynomial predictor basis
 * 
 * \param[in] encodedFilePath path of the polynomial encoded file
 * \param[in] outputFilePath name of the decoded output file
 * \return Decoded audio file
 */
void polynomialDecoder(const std::string encodedFilePath, const std::string outputFilePath);

/**
 * \brief Encodes file on a redundancy basis
 * \param[in] audioFile audioFile to encode
 * \param[in] outputFilePath name of the encoded output file
 * \return Vector of the residual values of the encoded file
 */
std::vector<std::vector<int>> redundancyPredictorEncoder(const AudioFile<double> audioFile, const std::string outputFilePath);

/**
 * \brief Decodes file on a redundancy basis
 * 
 * \param[in] encodedFilePath path of the redundancy encoded file
 * \param[in] outputFilePath name of the decoded output file
 * \return Decoded audio file
 */
void redundancyDecoder(const std::string encodedFilePath, const std::string outputFilePath);

//------------------------------------------------------------
//-------------------Constants--------------------------------
//------------------------------------------------------------
const uint16_t headerM = 200; 
const std::string resultDir = "../results";
const std::string matlabDir = "../matlab";

/*
    CodecArray:
    0 - firstOrderCodec
    1 - polynomialCodec
    2 - redundacyCodec
*/
typedef void (*DecoderFunction)(std::string encodedFilePath, std::string outputFilePath);
typedef std::vector<std::vector<int>> (*EncoderFunction)(AudioFile<double> audioFile, std::string outputFilePath);
const std::tuple<EncoderFunction,DecoderFunction> codecArray[] = 
{ 
    {&firstOrderPredictorEncoder, &firstOrderPredictorDecoder},     //  firstOrderCodec
    {&polynomialPredictorEncoder, &polynomialDecoder},                     //  polynomialCodec
    {&redundancyPredictorEncoder, &redundancyDecoder}                               //  redundacyCodec
};                                                                  /*!< tuple of which functions to call on main function */

/**
 * \brief Main method of lossless codec
 * 
 * Usage: ./codecLossless <AudioFilePath> <CodecNumber> <OutputFile>
 * 
 * \param[in] AudioFilePath input audio file
 * \param[in] CodecNumber target codec number
 * \param[in] OutputFile output file name for results
 */
int main(int argc, char *argv[]){
    if (argc < 4 || std::atoi(argv[2]) > 2 || std::atoi(argv[2]) < 0) {
        std::cout << "Usage: ./codecLossless <AudioFilePath> <CodecNumber> <OutputFile>" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "AudioFilePath: input audio file" << std::endl;
        std::cout << "CodecNumber: target codec number" << std::endl;
        std::cout << "OutputFilePath: output file name for results" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Codec options:" << std::endl;
        std::cout << "0 - First Order Codec" << std::endl;
        std::cout << "1 - Polynomial Codec" << std::endl;
        std::cout << "2 - Redundancy Codec" << std::endl;
        
        return -1;
    }

    std::string audioFile = argv[1];    
    uint8_t codecNumber = std::stoi(argv[2]); 
    std::string encoderOuputFile = resultDir + "/" + argv[3]; 
    
    AudioFile<double> audio;
    audio.load(audioFile);

    std::vector<double> entropies;
    std::vector<std::vector<int>> encodedResiduals;

    std::tuple<EncoderFunction,DecoderFunction> codec = codecArray[codecNumber];
    EncoderFunction encoder = std::get<0>(codec);
    DecoderFunction decoder = std::get<1>(codec);
    
    encodedResiduals = encoder(audio, encoderOuputFile);
    decoder(encoderOuputFile, encoderOuputFile);
    entropies = calculateEntropyAndResidualHistograms(encodedResiduals, audio.getBitDepth());
    std::cout << "--------Entropy results:--------" << std::endl;
    for(int i = 0; i < 2; i++)
    {
        std::cout << "Channel " <<  i  << " " << entropies[i] << std::endl;
    }

    return 0;
}

std::vector<double> calculateEntropyAndResidualHistograms(const std::vector<std::vector<int>> &residuals, const int bitDepth) {
    std::vector<int> channel1 = residuals[0];
    std::vector<int> channel2 = residuals[1];

    int numSamples = channel1.size();
    
    int nValues = std::pow(2,bitDepth);

    std::vector<std::map<int,int>> residualsMap;
    residualsMap.resize(2);
    //initialize maps containing sample count at 0
    for(int i = -nValues/2; i <= nValues/2; i++) {
        residualsMap[0][i] = 0;
    }
    for(int i = -nValues/2; i <= nValues/2; i++) {
        residualsMap[1][i] = 0;
    }
    //count residuals frequency
    int residual;
    for(int i = 0; i < numSamples; i++) {
        residual = residuals[0][i];
        residualsMap[0][residual]++;
    }
    for(int i = 0; i < numSamples; i++) {
        residual = residuals[1][i];
        residualsMap[1][residual]++;
    }

    //calculate probability of each amplitude for each channel
    std::vector<std::map<int,float>> samplesProb;
    samplesProb.resize(2);
    for(int i = 0; i < 2; i++) 
    {
        for(auto itr = residualsMap[i].begin(); itr != residualsMap[i].end(); itr++)
        {
            samplesProb[i][(*itr).first] = (*itr).second / (float) numSamples;
        }
    }

    //calculate entropy for each channel
    std::vector<double> entropies;
    entropies.resize(2);
    for(int i = 0; i < 2; i++) 
    {   
        for(auto itr = samplesProb[i].begin(); itr != samplesProb[i].end(); itr++)
        {
            if( (*itr).second != 0)
                entropies[i] += - (*itr).second * log2((*itr).second);
        }
    }

    const std::map<int,int> channel1Residuals = residualsMap[0];
    const std::map<int,int> channel2Residuals = residualsMap[1];

    std::ofstream channel1File( matlabDir + "/channel1.txt");
    std::ofstream channel2File( matlabDir + "/channel2.txt");

    //write channel1File (amplitude values)
    for(auto itr = channel1Residuals.begin(); itr != channel1Residuals.end(); itr++) {
        channel1File << (*itr).second << std::endl;        
    }

    //write channel2File (amplitude values)
    for(auto itr = channel2Residuals.begin(); itr != channel2Residuals.end(); itr++) {
        channel2File << (*itr).second << std::endl;        
    }

    channel1File.close();
    channel2File.close();

    return entropies;
}

std::vector<int> convertChannelAmplitudeToInteger(const std::vector<double> channel, const int bitDepth) {
    int maxValue = std::pow(2, bitDepth);
    std::vector<int> integerChannel;
    integerChannel.resize(channel.size());

    for(size_t i = 0; i < channel.size(); i++) {
        integerChannel[i] = channel[i] * maxValue;
    }
    return  integerChannel;
}

std::vector<double> revertChannelAmplitudeToDouble(const std::vector<int> channel, const int bitDepth) {
    int maxValue = std::pow(2, bitDepth);
    std::vector<double> doubleChannel;
    doubleChannel.resize(channel.size());

    for(size_t i = 0; i < channel.size(); i++) {
        doubleChannel[i] = channel[i] / (double) maxValue;
    }
    return  doubleChannel;
}

std::vector<std::vector<int>> redundancyPredictorEncoder(const AudioFile<double> audioFile, const std::string outputFilePath) {
    Golomb golomb(outputFilePath, BitStream::bs_mode::write, headerM);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int bitDepth = audioFile.getBitDepth();
    std::vector<int> left = convertChannelAmplitudeToInteger(audioFile.samples[0], bitDepth);
    std::vector<int> right = convertChannelAmplitudeToInteger(audioFile.samples[1], bitDepth);

    std::vector<std::vector<int>> encodedResiduals;
    encodedResiduals.resize(2);
    encodedResiduals[0].resize(numSamples);
    encodedResiduals[1].resize(numSamples);

    std::vector<int> x;
    x.resize(numSamples);
    std::vector<int> y;
    y.resize(numSamples);

    for(int i = 0; i < numSamples; i++){
        x[i] = floor((left[i] + right[i])/2);
        y[i] = left[i] - right[i]; 
    }

    int r, previous = 0;
    for(int i = 0; i < numSamples; i++) {
        r = x[i] - previous;
        encodedResiduals[0][i] = r;
        previous = x[i];
    }

    for(int i = 0; i < numSamples; i++) {
        r = y[i] - previous;
        encodedResiduals[1][i] = r;
        previous = y[i];
    }

    int m1 = golomb.getOtimizedM(encodedResiduals[0]);
    int m2 = golomb.getOtimizedM(encodedResiduals[1]);
    golomb.encodeNumber(m1);
    golomb.encodeNumber(m2);
    golomb.encodeNumber(numSamples);
    golomb.encodeNumber(bitDepth);

    golomb.setM(m1);
    for(size_t i = 0; i < encodedResiduals[0].size(); i++) {
        golomb.encodeNumber(encodedResiduals[0][i]);
    }

    golomb.setM(m2);
    for(size_t i = 0; i < encodedResiduals[1].size(); i++) {
        golomb.encodeNumber(encodedResiduals[1][i]);
    }
    return encodedResiduals;
}

void redundancyDecoder(const std::string encodedFilePath, const std::string outputFilePath) {
    Golomb decoder(encodedFilePath, BitStream::bs_mode::read, headerM);

    int32_t m1 = decoder.decodeNumber();
    int32_t m2 = decoder.decodeNumber();
    int32_t numSamples = decoder.decodeNumber();
    int32_t bitDepth = decoder.decodeNumber();

    decoder.setM(m1);
    std::vector<int> left;
    left.resize(numSamples);
    std::vector<int> right;
    right.resize(numSamples);

    std::vector<int> x;
    x.resize(numSamples);
    std::vector<int> y;
    y.resize(numSamples);

    int r, previous = 0;
    for(int i = 0; i < numSamples; i++){
        r = decoder.decodeNumber();
        x[i] = r + previous;
        previous = x[i];
    }

    decoder.setM(m2);
    for(int i = 0; i < numSamples; i++){
        r = decoder.decodeNumber();
        y[i] = r + previous;
        previous = y[i];
    }

    for(int i = 0; i < numSamples; i++){
        if(y[i] % 2 != 0){
            left[i] = y[i] + ((2*x[i] + 1 - y[i])/2);
            right[i] = (2*x[i] + 1 - y[i])/2;
        }
        else{
            left[i] = (y[i]/2) + x[i];
            right[i] = x[i] - (y[i]/2);
        }
    }

    std::vector<double> leftDouble = revertChannelAmplitudeToDouble(left,bitDepth);
    std::vector<double> rightDouble = revertChannelAmplitudeToDouble(right,bitDepth);
    
    AudioFile<double> audioFile;
    AudioFile<double>::AudioBuffer buffer;

    buffer.resize(2);
    buffer[0].resize(numSamples);
    buffer[1].resize(numSamples);

    for(int i = 0; i < numSamples; i++){
        buffer[0][i] = leftDouble[i];
    }
    for(int i = 0; i < numSamples; i++){
        buffer[1][i] = rightDouble[i];
    }
    audioFile.setNumSamplesPerChannel(numSamples);
    bool ok = audioFile.setAudioBuffer(buffer);
    
    if(!ok)
        std::cout << "ERROR: Unable to create audioFile!" << std::endl;

    audioFile.save(outputFilePath + ".wav");
}

std::vector<std::vector<int>> polynomialPredictorEncoder(const AudioFile<double> audioFile, const std::string outputFilePath) {
    Golomb golomb(outputFilePath, BitStream::bs_mode::write, headerM);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int bitDepth = audioFile.getBitDepth();
    std::vector<int> left = convertChannelAmplitudeToInteger(audioFile.samples[0], bitDepth);
    std::vector<int> right = convertChannelAmplitudeToInteger(audioFile.samples[1], bitDepth);

    std::vector<std::vector<int>> encodedResiduals;
    encodedResiduals.resize(2);
    encodedResiduals[0].resize(numSamples);
    encodedResiduals[1].resize(numSamples);

    int r = 0;
    int Xn_1, Xn_2, Xn_3;

    Xn_3 = left[0];
    r = 0 - left[0];
    encodedResiduals[0][0] = r;
    
    Xn_2 = left[1];
    r = Xn_3 - left[1];
    encodedResiduals[0][1] = r;

    r = (2*Xn_2 - Xn_3) - left[2];
    Xn_1 = left[2];
    encodedResiduals[0][2] = r;

    for(int i = 3; i < numSamples; i++){
        int sample = left[i];
        r = (3*Xn_1 - 3*Xn_2 + Xn_3) - sample; 
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = sample;

        encodedResiduals[0][i] = r;
    }

    Xn_3 = right[0];
    r = 0 - right[0];
    encodedResiduals[1][0] = r;
    
    Xn_2 = right[1];
    r = Xn_3 - right[1];
    encodedResiduals[1][1] = r;

    r = (2*Xn_2 - Xn_3) - right[2];
    Xn_1 = right[2];
    encodedResiduals[1][2] = r;

    for(int i = 3; i < numSamples; i++){
        int sample = right[i];
        r = (3*Xn_1 - 3*Xn_2 + Xn_3) - sample;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = sample;

        encodedResiduals[1][i] = r;
    }

    int32_t m1 = golomb.getOtimizedM(encodedResiduals[0]);
    int32_t m2 = golomb.getOtimizedM(encodedResiduals[1]);
    golomb.encodeNumber(m1);
    golomb.encodeNumber(m2);
    golomb.encodeNumber(numSamples);
    golomb.encodeNumber(bitDepth);

    golomb.setM(m1);
    for(size_t i = 0; i < encodedResiduals[0].size(); i++) {
        golomb.encodeNumber(encodedResiduals[0][i]);
    }

    golomb.setM(m2);
    for(size_t i = 0; i < encodedResiduals[1].size(); i++) {
        golomb.encodeNumber(encodedResiduals[1][i]);
    }
    
    return encodedResiduals;
}

void polynomialDecoder(const std::string encodedFilePath, const std::string outputFilePath) {
    Golomb decoder(encodedFilePath, BitStream::bs_mode::read, headerM);

    int32_t m1 = decoder.decodeNumber();
    int32_t m2 = decoder.decodeNumber();
    int32_t numSamples = decoder.decodeNumber();
    int32_t bitDepth = decoder.decodeNumber();

    //set m for channel 1
    decoder.setM(m1);

    std::vector<int> left;
    left.resize(numSamples);

    std::vector<int> right;
    right.resize(numSamples);

    int r = 0;
    int Xn_1, Xn_2, Xn_3;

    r = decoder.decodeNumber();
    left[0] = r;
    Xn_3 = left[0];

    r = decoder.decodeNumber();
    left[1] = Xn_3 - r;
    Xn_2 = left[1];

    r = decoder.decodeNumber();
    left[2] = (2*Xn_2 - Xn_3) - r;
    Xn_1 = left[2];

    for(int i = 3; i < numSamples; i++){
        r = decoder.decodeNumber();
        left[i] = (3*Xn_1 - 3*Xn_2 + Xn_3) - r;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = left[i];
    }

    //set m for channel 2
    decoder.setM(m2);
    
    r = decoder.decodeNumber();
    right[0] = r;
    Xn_3 = right[0];

    r = decoder.decodeNumber();
    right[1] = Xn_3 - r;
    Xn_2 = right[1];

    r = decoder.decodeNumber();
    right[2] = (2*Xn_2 - Xn_3) - r;
    Xn_1 = right[2];

    for(int i = 3; i < numSamples; i++){
        r = decoder.decodeNumber();
        right[i] = (3*Xn_1 - 3*Xn_2 + Xn_3) - r;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = right[i];

    } 
    std::vector<double> leftDouble = revertChannelAmplitudeToDouble(left,bitDepth);
    std::vector<double> rightDouble = revertChannelAmplitudeToDouble(right,bitDepth);
    
    AudioFile<double> audioFile;
    AudioFile<double>::AudioBuffer buffer;

    buffer.resize(2);
    buffer[0].resize(numSamples);
    buffer[1].resize(numSamples);

    for(int i = 0; i < numSamples; i++){
        buffer[0][i] = leftDouble[i];
    }
    for(int i = 0; i < numSamples; i++){
        buffer[1][i] = rightDouble[i];

    }
    audioFile.setNumSamplesPerChannel(numSamples);
    bool ok = audioFile.setAudioBuffer(buffer);

    if(!ok)
        std::cout << "ERROR: Unable to create audioFile!" << std::endl;

    audioFile.save(outputFilePath + ".wav");
}

std::vector<std::vector<int>> firstOrderPredictorEncoder(const AudioFile<double> audioFile, const std::string outputFilePath) {
    Golomb golomb(outputFilePath, BitStream::bs_mode::write, headerM);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int bitDepth = audioFile.getBitDepth();
    std::vector<int> left = convertChannelAmplitudeToInteger(audioFile.samples[0], bitDepth);
    std::vector<int> right = convertChannelAmplitudeToInteger(audioFile.samples[1], bitDepth);

    std::vector<std::vector<int>> encodedResiduals;
    encodedResiduals.resize(2);
    encodedResiduals[0].resize(numSamples);
    encodedResiduals[1].resize(numSamples);

    int r, previous = 0;
    for(int i = 0; i < numSamples; i++) {
        r = left[i] - previous;
        encodedResiduals[0][i] = r;
        previous = left[i];
    }

    for(int i = 0; i < numSamples; i++) {
        r = right[i] - previous;
        encodedResiduals[1][i] = r;
        previous = right[i];
    }

    int m1 = golomb.getOtimizedM(encodedResiduals[0]);
    int m2 = golomb.getOtimizedM(encodedResiduals[1]);

    golomb.encodeNumber(m1);
    golomb.encodeNumber(m2);
    golomb.encodeNumber(numSamples);
    golomb.encodeNumber(bitDepth);

    golomb.setM(m1);
    for(size_t i = 0; i < encodedResiduals[0].size(); i++) {
        golomb.encodeNumber(encodedResiduals[0][i]);
    }
    golomb.setM(m2);
    for(size_t i = 0; i < encodedResiduals[1].size(); i++) {
        golomb.encodeNumber(encodedResiduals[1][i]);
    }
    return encodedResiduals;
} 

void firstOrderPredictorDecoder(const std::string encodedFilePath, const std::string outputFilePath) {
    Golomb decoder(encodedFilePath, BitStream::bs_mode::read, headerM);
    int32_t m1 = decoder.decodeNumber();
    int32_t m2 = decoder.decodeNumber();
    int32_t numSamples = decoder.decodeNumber();
    int32_t bitDepth = decoder.decodeNumber();

    decoder.setM(m1);
    std::vector<int> left;
    left.resize(numSamples);

    std::vector<int> right;
    right.resize(numSamples);

    int r, previous = 0;

    for(int i = 0; i < numSamples; i++){
        r = decoder.decodeNumber();
        left[i] = r + previous;
        previous = left[i];
    }

    decoder.setM(m2);
    for(int i = 0; i < numSamples; i++){
        r = decoder.decodeNumber();
        right[i] = r + previous;
        previous = right[i];
    } 
    std::vector<double> leftDouble = revertChannelAmplitudeToDouble(left,bitDepth);
    std::vector<double> rightDouble = revertChannelAmplitudeToDouble(right,bitDepth);
    
    AudioFile<double> audioFile;
    AudioFile<double>::AudioBuffer buffer;

    buffer.resize(2);
    buffer[0].resize(numSamples);
    buffer[1].resize(numSamples);

    for(int i = 0; i < numSamples; i++){
        buffer[0][i] = leftDouble[i];
    }
    for(int i = 0; i < numSamples; i++){
        buffer[1][i] = rightDouble[i];
    }
    audioFile.setNumSamplesPerChannel(numSamples);
    bool ok = audioFile.setAudioBuffer(buffer);

    if(!ok)
        std::cout << "ERROR: Unable to create audioFile!" << std::endl;


    audioFile.save(outputFilePath + ".wav");
}

