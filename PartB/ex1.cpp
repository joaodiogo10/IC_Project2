#include "classes/Golomb.h"
#include "AudioFile/AudioFile.h"
#include <sstream>
#include <math.h>

const int m = 4;

void redundancy(AudioFile<double> audioFile){
    Golomb golomb("encoded", BitStream::bs_mode::write, m);

    int numChannels = audioFile.getNumChannels();
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

void prediction(AudioFile<double> audioFile){
    Golomb golomb("encoded2", BitStream::bs_mode::write, m);

    int numChannels = audioFile.getNumChannels();
    int numSamples = audioFile.getNumSamplesPerChannel();

    int p, previous;
    int pPred, r;
    previous = 0;

    
    for(int i = 0; i < numSamples; i++){
        p = audioFile.samples[i];
        pPred = audioFile.samples[i];
        r = i - previous;
        previous = i;
        pPred = r;
    }

    golomb.encodeNumber(pPred);

} 

int main(int argc, char * argv[]){
    AudioFile<double> audioFile;
    audioFile.load(argv[1]);

    redundancy(audioFile);

    prediction(audioFile);

    return 0;
}

