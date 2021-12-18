#! /bin/sh
images=`ls ./imageSamples`;

[ ! -d "./matlab" ] && mkdir matlab
[ ! -d "./results" ] && mkdir results

[ ! -d "./matlab/lossless" ] && mkdir matlab/lossless
[ ! -d "./results/lossless" ] && mkdir results/lossless

cd ./build ;

for img in $images
do
	tmp=`echo ${img} | cut -d '.' -f 1`;
 	
    echo "Image: ${img}"
	./encoderLossless "../imageSamples/${img}" "../results/lossless/${tmp}.txt";
    echo '\n'

    [ ! -d "../matlab/lossless/${tmp}" ] && mkdir ../matlab/lossless/${tmp}


    mv ../matlab/xAxis.txt ../matlab/lossless/${tmp}/
    mv ../matlab/YFrequence.txt ../matlab/lossless/${tmp}/
    mv ../matlab/UFrequence.txt ../matlab/lossless/${tmp}/
    mv ../matlab/VFrequence.txt ../matlab/lossless/${tmp}/

done