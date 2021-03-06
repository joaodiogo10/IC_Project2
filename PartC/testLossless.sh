#! /bin/sh
images=`ls ./imageSamples`;

[ ! -d "./matlab" ] && mkdir matlab
[ ! -d "./imageResults" ] && mkdir imageResults
[ ! -d "./compressedResults" ] && mkdir compressedResults

[ ! -d "./matlab/lossless" ] && mkdir matlab/lossless
[ ! -d "./imageResults/lossless" ] && mkdir imageResults/lossless
[ ! -d "./compressedResults/lossless" ] && mkdir compressedResults/lossless

cd ./build ;

for img in $images
do
	tmp=`echo ${img} | cut -d '.' -f 1`;
 	
    echo "Image: ${img}"
	./encoderLossless "../imageSamples/${img}" "../compressedResults/lossless/${tmp}" 6;
    echo '\n'

    [ ! -d "../matlab/lossless/${tmp}" ] && mkdir ../matlab/lossless/${tmp}
    [ ! -d "../imageResults/lossless/${tmp}" ] && mkdir ../imageResults/lossless/${tmp}


    mv xAxis.txt ../matlab/lossless/${tmp}/
    mv YFrequency.txt ../matlab/lossless/${tmp}/
    mv UFrequency.txt ../matlab/lossless/${tmp}/
    mv VFrequency.txt ../matlab/lossless/${tmp}/

    mv Y.png ../imageResults/lossless/${tmp}/
    mv U.png ../imageResults/lossless/${tmp}/
    mv V.png ../imageResults/lossless/${tmp}/
    mv UReduced.png ../imageResults/lossless/${tmp}/
    mv VReduced.png ../imageResults/lossless/${tmp}/

    ./decoderLossless "../compressedResults/lossless/${tmp}";

    mv YDecoded.png ../imageResults/lossless/${tmp}/
    mv UReducedDecoded.png ../imageResults/lossless/${tmp}/
    mv VReducedDecoded.png ../imageResults/lossless/${tmp}/
    mv RGBDecoded.png ../imageResults/lossless/${tmp}/

done