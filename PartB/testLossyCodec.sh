#! /bin/sh
audios=`ls ./audioSamples`;

[ ! -d "./resultsLossy" ] && mkdir resultsLossy

sum1=0;
sum2=0;
sum3=0;
total=0;
cd build;

for audio in $audios
do
    name=`echo ${audio} | cut -d '.' -f 1`;
    format=`echo ${audio} | cut -d '.' -f 2`;
	[ ! -d "../resultsLossy/${name}Results" ] && mkdir "../resultsLossy/${name}Results";
    
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

    firstOrderCodecResultFile="../resultsLossy/${name}Results/FirstOrderCodec";
	[ ! -d ${firstOrderCodecResultFile} ] && mkdir ${firstOrderCodecResultFile};
    mv "../resultsLossy/${name}_$1bits" ${firstOrderCodecResultFile}
    mv "../resultsLossy/${name}_$1bits.wav" ${firstOrderCodecResultFile}

    #---------------------------------------------------------------------
    echo "\n1- Polynomial Codec";
    tmp=$(./codecLossy "../audioSamples/${name}.${format}" 1 ${name}_$1bits $1);
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum2=$(echo "$sum2 + $ch1" | bc -l);
    sum2=$(echo "$sum2 + $ch2" | bc -l);

    polynomialCodecResultFile="../resultsLossy/${name}Results/PolynomialCodec";
	[ ! -d ${polynomialCodecResultFile} ] && mkdir ${polynomialCodecResultFile};
    mv "../resultsLossy/${name}_$1bits" ${polynomialCodecResultFile};
    mv "../resultsLossy/${name}_$1bits.wav" ${polynomialCodecResultFile};

    #---------------------------------------------------------------------
    echo "\n2- Redundancy Codec";
    tmp=$(./codecLossy "../audioSamples/${name}.${format}" 2 "${name}_$1bits" $1);
    echo "$tmp"
    ch1=$(echo $tmp | cut -f5 -d" ");
    ch2=$(echo $tmp | cut -f8 -d" ");
    sum3=$(echo "$sum3 + $ch1" | bc -l);
    sum3=$(echo "$sum3 + $ch2" | bc -l);

    RedundancyCodecResultFile="../resultsLossy/${name}Results/RedundancyCodec";
	[ ! -d ${RedundancyCodecResultFile} ] && mkdir ${RedundancyCodecResultFile};
    mv "../resultsLossy/${name}_$1bits" ${RedundancyCodecResultFile};
    mv "../resultsLossy/${name}_$1bits.wav" ${RedundancyCodecResultFile};

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
