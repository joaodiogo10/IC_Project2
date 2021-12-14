#include "../PartA_1/classes/Golomb.h"
#include "AudioFile/AudioFile.h"
#include <sstream>
#include <math.h>

const int m = 7;

void redundancy(AudioFile<double> audioFile){
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

// void prediction(AudioFile<double> audioFile){
//     Golomb golomb("encoded2", BitStream::bs_mode::write, m);

//     int numSamples = audioFile.getNumSamplesPerChannel();

//     std::vector<double> left = audioFile.samples[0];
//     std::vector<double> right = audioFile.samples[1];

//     int r, previous = 0;
    
//     for(int i = 0; i < numSamples; i++){
//         r = left[i] - previous;
//         golomb.encodeNumber(r);
//         previous = left[i];
//     }
// } 

void polynomialPredictor(AudioFile<double> audioFile) {
    Golomb golomb("encoded2", BitStream::bs_mode::write, m);

    int numSamples = audioFile.getNumSamplesPerChannel();

    std::vector<double> left = audioFile.samples[0];
    std::vector<double> right = audioFile.samples[1];

    int r, previous = 0;
    int Xn_1, Xn_2, Xn_3;

    Xn_1 = left[0];
    r = 0 - left[0];
    golomb.encodeNumber(r);
    
    Xn_2 = left[1];
    r = Xn_1 - left[1];
    golomb.encodeNumber(r);

    r = (2*Xn_1 - Xn_2) - left[2];
    Xn_3 = left[2];
    golomb.encodeNumber(r);

    for(int i = 3; i < numSamples; i++){
        int sample = left[i];
        r = (3*Xn_1 - 3*Xn_2 + Xn_3) - sample;
        Xn_3 = Xn_2;
        Xn_2 = Xn_1;
        Xn_1 = sample;

        golomb.encodeNumber(r);
    }
}

void predictorDecoder(char * filepath){
    Golomb golomb("decodedFile2", BitStream::bs_mode::read, m);
    
}

int main(int argc, char * argv[]){
    AudioFile<double> audioFile;
    audioFile.load(argv[1]);

    redundancy(audioFile);

    polynomialPredictor(audioFile);

    return 0;
}

