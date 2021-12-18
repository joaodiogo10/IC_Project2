#include "../PartA_1/classes/Golomb.h"
#include "AudioFile/AudioFile.h"
#include <sstream>
#include <math.h>
#include <map>

const uint16_t headerM = 200;
std::string encoderOuputFile = "encodedFile"; 
void writeMatlabVectorFiles(const std::vector<std::map<int,int>> &residualsMap) {
    const std::map<int,int> channel1 = residualsMap[0];
    const std::map<int,int> channel2 = residualsMap[1];

    std::ofstream channel1File("matlab/channel1.txt");
    std::ofstream channel2File("matlab/channel2.txt");

    //write channel1File (amplitude values)
    for(auto itr = channel1.begin(); itr != channel1.end(); itr++) {
        channel1File << (*itr).second << std::endl;        
    }

    //write channel2File (amplitude values)
    for(auto itr = channel2.begin(); itr != channel2.end(); itr++) {
        channel2File << (*itr).second << std::endl;        
    }

    channel1File.close();
    channel2File.close();
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

    writeMatlabVectorFiles(residualsMap);
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

std::vector<double> revertChannelAmplitudeToDouble(const std::vector<int> channel, const int bitDepth){
    int maxValue = std::pow(2, bitDepth);
    std::vector<double> doubleChannel;
    doubleChannel.resize(channel.size());

    for(size_t i = 0; i < channel.size(); i++) {
        doubleChannel[i] = channel[i] / (double) maxValue;
    }
    return  doubleChannel;
}

std::vector<int> redundancy(AudioFile<double> audioFile){
    Golomb golomb("encodedRed", BitStream::bs_mode::write, headerM);

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
        encodedResiduals[i] = r;

        golomb.encodeNumber(r);
        previous = x[i];
    }

    for(int i = 0; i < numSamples; i++) {
        r = y[i] - previous;
        encodedResiduals[i+numSamples] = r;

        golomb.encodeNumber(r);
        previous = y[i];
    }

    return encodedResiduals;
}

void redundancyDecoder(std::string filePath){
    Golomb decoder(filePath, BitStream::bs_mode::read, headerM);

    int32_t M = decoder.decodeNumber();
    int32_t numSamples = decoder.decodeNumber();
    int32_t bitDepth = decoder.decodeNumber();

    decoder.setM(M);

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


    audioFile.save("decodedFileRedundancy.wav");
}


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

std::vector<std::vector<int>> firstOrderPredictorEncoder(AudioFile<double> audioFile) {
    Golomb golomb(encoderOuputFile, BitStream::bs_mode::write, headerM);

    int numSamples = audioFile.getNumSamplesPerChannel();
    int bitDepth = audioFile.getBitDepth();
    std::vector<int> left = convertChannelAmplitudeToInteger(audioFile.samples[0], bitDepth);
    std::vector<int> right = convertChannelAmplitudeToInteger(audioFile.samples[1], bitDepth);

    std::vector<std::vector<int>> encodedResiduals;
    encodedResiduals.resize(2);
    encodedResiduals[0].resize(numSamples*2);
    encodedResiduals[1].resize(numSamples*2);

    int m = golomb.getOtimizedM(left);
    golomb.encodeNumber(m);
    golomb.encodeNumber(numSamples);
    golomb.encodeNumber(bitDepth);

    golomb.setM(m);
    int r, previous = 0;
    for(int i = 0; i < numSamples; i++) {
        r = left[i] - previous;
        encodedResiduals[0][i] = r;

        golomb.encodeNumber(r);
        previous = left[i];
    }

    for(int i = 0; i < numSamples; i++) {
        r = right[i] - previous;
        encodedResiduals[1][i] = r;

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

int main(int argc, char *argv[]){
/*     std::cout << "Do you want to encode or decode the file?" << std::endl;
    std::cout << "1 - encode" << std::endl;
    std::cout << "2 - decode" << std::endl;
    int choice1;
    std::cin >> choice1;

    if(choice1 == 1){
        int choice2;
        AudioFile<double> audioFile;
        std::cout << "predictor or redundancy?" << std::endl;
        std::cout << "1 - predictor" << std::endl;
        std::cout << "2 - redundancy" << std::endl;
        std::cin >> choice2;

        if(choice2 == 1){
            std::string path;
            std::cout << "choose the path of the file to encode" << std::endl;
            std::cin >> path;
            audioFile.load(path);
            polynomialPredictor(audioFile);
        }

        else if(choice2 == 2){
            std::string path;
            std::cout << "choose the path of the file to encode" << std::endl;
            std::cin >> path;
            audioFile.load(path);
            redundancy(audioFile);
        }

    } */
    AudioFile<double> audioFile;
    audioFile.load(argv[1]);
    std::vector<std::vector<int>> encodedResiduals = firstOrderPredictorEncoder(audioFile);
    firstOrderPredictorDecoder(encoderOuputFile);
    
    std::vector<double> entropies;
    entropies = calculateEntropyAndResidualHistograms(encodedResiduals, audioFile.getBitDepth());
    for(int i = 0; i < 2; i++)
    {
        std::cout << entropies[i] << std::endl;
        }
/*     
        int choice2;
        std::cout << "predictor or redundancy?" << std::endl;
        std::cout << "1 - predictor" << std::endl;
        std::cout << "2 - redundancy" << std::endl;
        std::cin >> choice2;

        if(choice2 == 1){
            std::string path;
            std::cout << "choose the path of the file to decode" << std::endl;
            std::cin >> path;
            polynomialDecoder(path);
        }

        else if(choice2 == 2){
            std::string path;
            std::cout << "choose the path of the file to encode" << std::endl;
            std::cin >> path;
            redundancyDecoder(path);
        }
    } */

    return 0;
}

