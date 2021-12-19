#! /bin/sh
audios=`ls ./audioSamples`;

[ ! -d "./matlab" ] && mkdir matlab
[ ! -d "./results" ] && mkdir results


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
    echo "\n1- First Order Codec";
    echo ./codecLossless "../audioSamples/${name}.${format}" 0 "${name}";
	./codecLossless "../audioSamples/${name}.${format}" 0 "${name}";

    firstOrderCodecResultFile="../results/${name}Results/FirstOrderCodec";
	[ ! -d ${firstOrderCodecResultFile} ] && mkdir ${firstOrderCodecResultFile};
    mv "../results/${name}" ${firstOrderCodecResultFile}
    mv "../results/${name}.wav" ${firstOrderCodecResultFile}
    
    firstOrderCodecMatlabFile="../matlab/${name}Matlab/FirstOrderCodec";
	[ ! -d ${firstOrderCodecMatlabFile} ] && mkdir ${firstOrderCodecMatlabFile};
    mv "../matlab/channel1.txt" ${firstOrderCodecMatlabFile};
    mv "../matlab/channel2.txt" ${firstOrderCodecMatlabFile};

    #---------------------------------------------------------------------
    echo "\n2- Polynomial Codec";
    echo ./codecLossless "../audioSamples/${name}.${format}" 1 "${name}";
    ./codecLossless "../audioSamples/${name}.${format}" 1 "${name}";

    polynomialCodecResultFile="../results/${name}Results/PolynomialCodec";
	[ ! -d ${polynomialCodecResultFile} ] && mkdir ${polynomialCodecResultFile};
    mv "../results/${name}" ${polynomialCodecResultFile};
    mv "../results/${name}.wav" ${polynomialCodecResultFile};
    
    polynomialCodecMatlabFile="../matlab/${name}Matlab/PolynomialCodec";
	[ ! -d ${polynomialCodecMatlabFile} ] && mkdir ${polynomialCodecMatlabFile};
    mv "../matlab/channel1.txt" ${polynomialCodecMatlabFile};
    mv "../matlab/channel2.txt" ${polynomialCodecMatlabFile};

    #---------------------------------------------------------------------
    echo "\n3- Redundancy Codec";
    echo ./codecLossless "../audioSamples/${name}.${format}" 2 "${name}";
    ./codecLossless "../audioSamples/${name}.${format}" 2 "${name}";

    RedundancyCodecResultFile="../results/${name}Results/RedundancyCodec";
	[ ! -d ${RedundancyCodecResultFile} ] && mkdir ${RedundancyCodecResultFile};
    mv "../results/${name}" ${RedundancyCodecResultFile};
    mv "../results/${name}.wav" ${RedundancyCodecResultFile};

    RedundancyCodecMatlabFile="../matlab/${name}Matlab/RedundancyCodec";
	[ ! -d ${RedundancyCodecMatlabFile} ] && mkdir ${RedundancyCodecMatlabFile};
    mv "../matlab/channel1.txt" ${RedundancyCodecMatlabFile};
    mv "../matlab/channel2.txt" ${RedundancyCodecMatlabFile};
done

cd ..;
