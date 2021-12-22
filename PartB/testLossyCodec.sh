#! /bin/sh
audios=`ls ./audioSamples`;

[ ! -d "./matlabLossy" ] && mkdir matlabLossy;
[ ! -d "./resultsLossy" ] && mkdir resultsLossy;
[ ! -d "./matlabLossy/$1_bits" ] && mkdir "./matlabLossy/$1_bits";
[ ! -d "./resultsLossy/$1_bits" ] && mkdir "./resultsLossy/$1_bits";

sum1=0;
sum2=0;
sum3=0;
total=0;
cd build;

for audio in $audios
do
    name=`echo ${audio} | cut -d '.' -f 1`;
    format=`echo ${audio} | cut -d '.' -f 2`;
	[ ! -d "../resultsLossy/$1_bits/${name}Results" ] && mkdir "../resultsLossy/$1_bits/${name}Results";
    [ ! -d "../matlabLossy/$1_bits/${name}Matlab" ] && mkdir "../matlabLossy/$1_bits/${name}Matlab";

    echo "\n------${name}------";
    echo "\n../audioSamples/${name}.${format}";
    
    #---------------------------------------------------------------------
    echo "\n0- First Order Codec";  
	tmp=$(./codecLossy "../audioSamples/${name}.${format}" 0 ${name}_$1bits $1);
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum1=$(echo "$sum1 + $ch1" | bc -l);
    sum1=$(echo "$sum1 + $ch2" | bc -l);

    firstOrderCodecResultFile="../resultsLossy/$1_bits/${name}Results/FirstOrderCodec";
	[ ! -d ${firstOrderCodecResultFile} ] && mkdir ${firstOrderCodecResultFile};
    mv "../resultsLossy/${name}_$1bits" ${firstOrderCodecResultFile}
    mv "../resultsLossy/${name}_$1bits.wav" ${firstOrderCodecResultFile}

    firstOrderCodecMatlabFile="../matlabLossy/$1_bits/${name}Matlab/FirstOrderCodec";
	[ ! -d ${firstOrderCodecMatlabFile} ] && mkdir ${firstOrderCodecMatlabFile};
    mv "../matlabLossy/channel1.txt" ${firstOrderCodecMatlabFile};
    mv "../matlabLossy/channel2.txt" ${firstOrderCodecMatlabFile};

    #---------------------------------------------------------------------
    echo "\n1- Polynomial Codec";
    tmp=$(./codecLossy "../audioSamples/${name}.${format}" 1 ${name}_$1bits $1);
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum2=$(echo "$sum2 + $ch1" | bc -l);
    sum2=$(echo "$sum2 + $ch2" | bc -l);

    polynomialCodecResultFile="../resultsLossy/$1_bits/${name}Results/PolynomialCodec";
	[ ! -d ${polynomialCodecResultFile} ] && mkdir ${polynomialCodecResultFile};
    mv "../resultsLossy/${name}_$1bits" ${polynomialCodecResultFile};
    mv "../resultsLossy/${name}_$1bits.wav" ${polynomialCodecResultFile};
    
    polynomialCodecMatlabFile="../matlabLossy/$1_bits/${name}Matlab/PolynomialCodec";
	[ ! -d ${polynomialCodecMatlabFile} ] && mkdir ${polynomialCodecMatlabFile};
    mv "../matlabLossy/channel1.txt" ${polynomialCodecMatlabFile};
    mv "../matlabLossy/channel2.txt" ${polynomialCodecMatlabFile};

    #---------------------------------------------------------------------
    echo "\n2- Redundancy Codec";
    tmp=$(./codecLossy "../audioSamples/${name}.${format}" 2 "${name}_$1bits" $1);
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum3=$(echo "$sum3 + $ch1" | bc -l);
    sum3=$(echo "$sum3 + $ch2" | bc -l);

    RedundancyCodecResultFile="../resultsLossy/$1_bits/${name}Results/RedundancyCodec";
	[ ! -d ${RedundancyCodecResultFile} ] && mkdir ${RedundancyCodecResultFile};
    mv "../resultsLossy/${name}_$1bits" ${RedundancyCodecResultFile};
    mv "../resultsLossy/${name}_$1bits.wav" ${RedundancyCodecResultFile};

    RedundancyCodecMatlabFile="../matlabLossy/$1_bits/${name}Matlab/RedundancyCodec";
	[ ! -d ${RedundancyCodecMatlabFile} ] && mkdir ${RedundancyCodecMatlabFile};
    mv "../matlabLossy/channel1.txt" ${RedundancyCodecMatlabFile};
    mv "../matlabLossy/channel2.txt" ${RedundancyCodecMatlabFile};

    total=$((total+2));
done
mean1=$(echo "$sum1 / $total" | bc -l);
mean2=$(echo "$sum2 / $total" | bc -l);
mean3=$(echo "$sum3 / $total" | bc -l);

echo "\nMean entropy 0- First Order Codec"
echo $mean1

echo "\nMean entropy 1- Polynomial Codec"
echo $mean2

echo "\nMean entropy 2- Redundancy Codec"
echo $mean3


cd ..;
