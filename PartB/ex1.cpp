#include "../PartA_1/classes/Golomb.h"
#include "AudioFile/AudioFile.h"
#include <sstream>
#include <math.h>

const uint16_t headerM = 200;
const std::string encoderOuputFile = "encodedPol";

std::vector<int> convertChannelAmplitudeToInteger(const std::vector<double> channel, const int bitDepth) {
    int maxValue = std::pow(2, bitDepth);
    std::vector<int> integerChannel;
    integerChannel.resize(channel.size());

    for(size_t i = 0; i < channel.size(); i++) {
        integerChannel[i] = channel[i] * maxValue;
    }
    return  integerChannel;
}

std::vector<double> revertChannelAmplitudeToDouble(const std::vector<int> channel, const int bitDepth){
    int maxValue = std::pow(2, bitDepth);
    std::vector<double> doubleChannel;
    doubleChannel.resize(channel.size());

    for(size_t i = 0; i < channel.size(); i++) {
        doubleChannel[i] = channel[i] / (double) maxValue;
    }
    return  doubleChannel;
}

/* void redundancy(AudioFile<double> audioFile){
    Golomb golomb("encoded", BitStream::bs_mode::write, m);

    //int numChannels = audioFile.getNumChannels();
    int numSamples = audioFile.getNumSamplesPerChannel();

    std::vector<double> left = audioFile.samples[0];
    std::vector<double> right = audioFile.samples[1];

    int x = 0;
    int y = 0;

    for(int i = 0; i < numSamples; i++){
        x += floor((left[i] + right[i])/2);
        y += left[i] - right[i]; 
    }

    golomb.encodeNumber(x);
    golomb.encodeNumber(y);
}

void redundancyDecoder(char * filePath){

}
 */
std::vector<int> polynomialPredictor(AudioFile<double> audioFile) {
    Golomb golomb("encodedPol", BitStream::bs_mode::write, headerM);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int bitDepth = audioFile.getBitDepth();
    std::vector<int> left = convertChannelAmplitudeToInteger(audioFile.samples[0], bitDepth);
    std::vector<int> right = convertChannelAmplitudeToInteger(audioFile.samples[1], bitDepth);

    std::vector<int> encodedResiduals;
    encodedResiduals.resize(numSamples*2);
    int m = golomb.getOtimizedM(left);
    golomb.encodeNumber(m);
    golomb.encodeNumber(numSamples);
    golomb.encodeNumber(bitDepth);

    golomb.setM(m);

    int r = 0;
    int Xn_1, Xn_2, Xn_3;

    Xn_1 = left[0];
    r = 0 - left[0];
    encodedResiduals[0] = r;
    golomb.encodeNumber(r);
    
    Xn_2 = left[1];
    r = Xn_1 - left[1];
    encodedResiduals[1] = r;
    golomb.encodeNumber(r);

    r = (2*Xn_1 - Xn_2) - left[2];
    Xn_3 = left[2];
    encodedResiduals[2] = r;
    golomb.encodeNumber(r);

    for(int i = 3; i < numSamples; i++){
        int sample = left[i];
        r = (3*Xn_1 - 3*Xn_2 + Xn_3) - sample;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = sample;

        encodedResiduals[i] = r;
        golomb.encodeNumber(r);
    }

    Xn_1 = right[0];
    r = 0 - right[0];
    encodedResiduals[numSamples] = r;
    golomb.encodeNumber(r);
    
    Xn_2 = right[1];
    r = Xn_1 - right[1];
    encodedResiduals[1+numSamples] = r;
    golomb.encodeNumber(r);

    r = (2*Xn_1 - Xn_2) - right[2];
    Xn_3 = right[2];
    encodedResiduals[2+numSamples] = r;
    golomb.encodeNumber(r);

    for(int i = 3; i < numSamples; i++){
        int sample = right[i];
        r = (3*Xn_1 - 3*Xn_2 + Xn_3) - sample;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = sample;

        encodedResiduals[i+numSamples] = r;
        golomb.encodeNumber(r);
    }

    return encodedResiduals;
}

void polynomialDecoder(std::string filePath){
    Golomb decoder(filePath, BitStream::bs_mode::read, headerM);

    int32_t M = decoder.decodeNumber();
    int32_t numSamples = decoder.decodeNumber();
    int32_t bitDepth = decoder.decodeNumber();

    decoder.setM(M);

    std::vector<int> left;
    left.resize(numSamples);

    std::vector<int> right;
    right.resize(numSamples);

    int r = 0;
    int Xn_1, Xn_2, Xn_3;

    r = decoder.decodeNumber();
    left[0] = r;
    Xn_1 = left[0];

    r = decoder.decodeNumber();
    left[1] = Xn_1 - r;
    Xn_2 = left[1];

    r = decoder.decodeNumber();
    left[2] = (2*Xn_1 - Xn_2) - r;
    Xn_3 = left[2];

    // Xn_1 = left[0];
    // r = 0 - left[0];
    // encodedResiduals[0] = r;
    // golomb.encodeNumber(r);
    
    // Xn_2 = left[1];
    // r = Xn_1 - left[1];
    // encodedResiduals[1] = r;
    // golomb.encodeNumber(r);

    // r = (2*Xn_1 - Xn_2) - left[2];
    // Xn_3 = left[2];
    // encodedResiduals[2] = r;
    // golomb.encodeNumber(r);

    // int sample = left[i];
    //     r = (3*Xn_1 - 3*Xn_2 + Xn_3) - sample;
    //     Xn_3 = Xn_2;
    //     Xn_2 = Xn_1;
    //     Xn_1 = sample;

    //     encodedResiduals[i] = r;
    //     golomb.encodeNumber(r);

    for(int i = 3; i < numSamples; i++){
        r = decoder.decodeNumber();
        left[i] = (3*Xn_1 - 3*Xn_2 + Xn_3) - r;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = left[i];
    }

    r = decoder.decodeNumber();
    right[0] = r;
    Xn_1 = right[0];

    r = decoder.decodeNumber();
    right[1] = Xn_1 - r;
    Xn_2 = right[1];

    r = decoder.decodeNumber();
    right[2] = (2*Xn_1 - Xn_2) - r;
    Xn_3 = right[2];

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


    audioFile.save("decodedFilePolynomial.wav");
}

std::vector<int> firstOrderPredictorEncoder(AudioFile<double> audioFile) {
    Golomb golomb(encoderOuputFile, BitStream::bs_mode::write, headerM);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int bitDepth = audioFile.getBitDepth();
    std::vector<int> left = convertChannelAmplitudeToInteger(audioFile.samples[0], bitDepth);
    std::vector<int> right = convertChannelAmplitudeToInteger(audioFile.samples[1], bitDepth);

    std::vector<int> encodedResiduals;
    encodedResiduals.resize(numSamples*2);

    int m = golomb.getOtimizedM(left);
    golomb.encodeNumber(m);
    golomb.encodeNumber(numSamples);
    golomb.encodeNumber(bitDepth);

    golomb.setM(m);
    int r, previous = 0;
    for(int i = 0; i < numSamples; i++) {
        r = left[i] - previous;
        encodedResiduals[i] = r;

        golomb.encodeNumber(r);
        previous = left[i];
    }

    for(int i = 0; i < numSamples; i++) {
        r = right[i] - previous;
        encodedResiduals[i+numSamples] = r;

        golomb.encodeNumber(r);
        previous = right[i];
    }

    return encodedResiduals;
} 

void firstOrderPredictorDecoder(std::string filepath) {
    Golomb decoder(filepath, BitStream::bs_mode::read, headerM);

    int32_t M = decoder.decodeNumber();
    int32_t numSamples = decoder.decodeNumber();
    int32_t bitDepth = decoder.decodeNumber();

    decoder.setM(M);

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


    audioFile.save("decodedFilePredictor.wav");
}

int main(int argc, char * argv[]){
    AudioFile<double> audioFile;
    audioFile.load(argv[1]);

    std::string filePath = argv[1];
    polynomialPredictor(audioFile);
    polynomialDecoder(encoderOuputFile);

    return 0;
}

