#! /bin/sh
images=`ls ./imageSamples`;

[ ! -d "./matlab" ] && mkdir matlab
[ ! -d "./imageResults" ] && mkdir imageResults
[ ! -d "./compressedResults" ] && mkdir compressedResults

[ ! -d "./matlab/lossy" ] && mkdir matlab/lossy
[ ! -d "./imageResults/lossy" ] && mkdir imageResults/lossy
[ ! -d "./compressedResults/lossy" ] && mkdir compressedResults/lossy

cd ./build ;

for img in $images
do
	tmp=`echo ${img} | cut -d '.' -f 1`;
 	
    echo "Image: ${img}"
	./encoderLossy "../imageSamples/${img}" "../compressedResults/lossy/${tmp}" 3 5 5 5;
    echo '\n'

    [ ! -d "../matlab/lossy/${tmp}" ] && mkdir ../matlab/lossy/${tmp}
    [ ! -d "../imageResults/lossy/${tmp}" ] && mkdir ../imageResults/lossy/${tmp}

    mv ../matlab/xAxis.txt ../matlab/lossy/${tmp}/
    mv ../matlab/YFrequency.txt ../matlab/lossy/${tmp}/
    mv ../matlab/UFrequency.txt ../matlab/lossy/${tmp}/
    mv ../matlab/VFrequency.txt ../matlab/lossy/${tmp}/

    mv Y.png ../imageResults/lossy/${tmp}/
    mv U.png ../imageResults/lossy/${tmp}/
    mv V.png ../imageResults/lossy/${tmp}/
    mv UReduced.png ../imageResults/lossy/${tmp}/
    mv VReduced.png ../imageResults/lossy/${tmp}/

    ./decoderLossy "../compressedResults/lossy/${tmp}";

    mv YDecoded.png ../imageResults/lossy/${tmp}/
    mv UReducedDecoded.png ../imageResults/lossy/${tmp}/
    mv VReducedDecoded.png ../imageResults/lossy/${tmp}/
    
done