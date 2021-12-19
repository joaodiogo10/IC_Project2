#! /bin/sh
audios=`ls ./audioSamples`;

[ ! -d "./matlab" ] && mkdir matlab
[ ! -d "./results" ] && mkdir results

sum1=0;
sum2=0;
sum3=0;
total=0;
cd build;
for audio in $audios
do
    name=`echo ${audio} | cut -d '.' -f 1`;
    format=`echo ${audio} | cut -d '.' -f 2`;
	[ ! -d "../results/${name}Results" ] && mkdir "../results/${name}Results";
	[ ! -d "../matlab/${name}Matlab" ] && mkdir "../matlab/${name}Matlab";
    
    echo "\n------${name}------";
    echo "\n../audioSamples/${name}.${format}";
    
    #---------------------------------------------------------------------
    echo "\n0- First Order Codec";
	tmp=$(./codecLossless "../audioSamples/${name}.${format}" 0 "${name}");
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum1=$(echo "$sum1 + $ch1" | bc -l);
    sum1=$(echo "$sum1 + $ch2" | bc -l);

    firstOrderCodecResultFile="../results/${name}Results/FirstOrderCodec";
	[ ! -d ${firstOrderCodecResultFile} ] && mkdir ${firstOrderCodecResultFile};
    mv "../results/${name}" ${firstOrderCodecResultFile}
    mv "../results/${name}.wav" ${firstOrderCodecResultFile}
    
    firstOrderCodecMatlabFile="../matlab/${name}Matlab/FirstOrderCodec";
	[ ! -d ${firstOrderCodecMatlabFile} ] && mkdir ${firstOrderCodecMatlabFile};
    mv "../matlab/channel1.txt" ${firstOrderCodecMatlabFile};
    mv "../matlab/channel2.txt" ${firstOrderCodecMatlabFile};

    #---------------------------------------------------------------------
    echo "\n1- Polynomial Codec";
    tmp=$(./codecLossless "../audioSamples/${name}.${format}" 1 "${name}");
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum2=$(echo "$sum2 + $ch1" | bc -l);
    sum2=$(echo "$sum2 + $ch2" | bc -l);

    polynomialCodecResultFile="../results/${name}Results/PolynomialCodec";
	[ ! -d ${polynomialCodecResultFile} ] && mkdir ${polynomialCodecResultFile};
    mv "../results/${name}" ${polynomialCodecResultFile};
    mv "../results/${name}.wav" ${polynomialCodecResultFile};
    
    polynomialCodecMatlabFile="../matlab/${name}Matlab/PolynomialCodec";
	[ ! -d ${polynomialCodecMatlabFile} ] && mkdir ${polynomialCodecMatlabFile};
    mv "../matlab/channel1.txt" ${polynomialCodecMatlabFile};
    mv "../matlab/channel2.txt" ${polynomialCodecMatlabFile};

    #---------------------------------------------------------------------
    echo "\n2- Redundancy Codec";
    tmp=$(./codecLossless "../audioSamples/${name}.${format}" 2 "${name}");
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum3=$(echo "$sum3 + $ch1" | bc -l);
    sum3=$(echo "$sum3 + $ch2" | bc -l);

    RedundancyCodecResultFile="../results/${name}Results/RedundancyCodec";
	[ ! -d ${RedundancyCodecResultFile} ] && mkdir ${RedundancyCodecResultFile};
    mv "../results/${name}" ${RedundancyCodecResultFile};
    mv "../results/${name}.wav" ${RedundancyCodecResultFile};

    RedundancyCodecMatlabFile="../matlab/${name}Matlab/RedundancyCodec";
	[ ! -d ${RedundancyCodecMatlabFile} ] && mkdir ${RedundancyCodecMatlabFile};
    mv "../matlab/channel1.txt" ${RedundancyCodecMatlabFile};
    mv "../matlab/channel2.txt" ${RedundancyCodecMatlabFile};

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
