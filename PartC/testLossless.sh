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
	./encoderLossless "../imageSamples/${img}" "../compressedResults/lossless/${tmp}";
    echo '\n'

    [ ! -d "../matlab/lossless/${tmp}" ] && mkdir ../matlab/lossless/${tmp}
    [ ! -d "../imageResults/lossless/${tmp}" ] && mkdir ../imageResults/lossless/${tmp}


    mv ../matlab/xAxis.txt ../matlab/lossless/${tmp}/
    mv ../matlab/YFrequency.txt ../matlab/lossless/${tmp}/
    mv ../matlab/UFrequency.txt ../matlab/lossless/${tmp}/
    mv ../matlab/VFrequency.txt ../matlab/lossless/${tmp}/

    mv Y.png ../imageResults/lossless/${tmp}/
    mv U.png ../imageResults/lossless/${tmp}/
    mv V.png ../imageResults/lossless/${tmp}/
    mv UReduced.png ../imageResults/lossless/${tmp}/
    mv VReduced.png ../imageResults/lossless/${tmp}/
    
done