#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

/** \file 
 *  Lossless encoder. \n
 *  Image transformations are saved in files. \n
 *  Frequency of residuals are written in files, under matlab/lossless/ImageName. \n
 *  File with the encoded information is written under results. \n
*/

/**
* \brief Converts RGB to YUV.
* 
* For each pixel of the source image, that is each position of the Mat where values for R, G and B are stored, it applies the following conversion: \n
* Y = 0.299 * B + 0.587 * G + 0.114 * R \n
* U = 128 - 0.168736 * B - 0.331264 * G + 0.5 * R \n
* V = 128 + 0.5 * B - 0.418688 * G - 0.081312 * R \n
* \n
* Each value is saved in the respective Mat.
* 
* \param[in] source \ref cv::Mat of the source RGB image.
* \param[in,out] YComponent \ref cv::Mat to store the values of Y.
* \param[in,out] UComponent \ref cv::Mat to store the values of U.
* \param[in,out] VComponent \ref cv::Mat to store the values of V.
*/
void convertToYUV(const cv::Mat &source, cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent);

/**
* \brief Reduces YUV to YUV 4:2:0.
* 
* Removes odd rows and columns of the \p UComponent and \p VComponent chrominance components, producing a reduction in the data rate. \n
* \n
* Each value is saved in the respective Mat.
* 
* \param[in] YComponent \ref cv::Mat with the values of Y.
* \param[in] UComponent \ref cv::Mat with the values of U.
* \param[in] VComponent \ref cv::Mat with the values of V.
* \param[in,out] UComponentReduced \ref cv::Mat to store the sub-samples of U.
* \param[in,out] VComponentReduced \ref cv::Mat to store the sub-samples of V.
*/
void convertTo420(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);

/**
* \brief First of the seven linear predictors of the lossless mode of JPEG. It calculates the residuals based on this mode.
* 
* It subtracts the previous pixel intensity to the current one. \n
* In the case of the first column it considers the previous value to be zero. \n
* \n
* Each residual value is saved in the respective Mat.
* 
* \param[in] YComponent \ref cv::Mat with the values of Y.
* \param[in] UComponentReduced \ref cv::Mat with the sub-samples of U.
* \param[in] VComponentReduced \ref cv::Mat with the sub-samples of V.
* \param[in,out] YPredictor \ref cv::Mat to store the residuals of the Y component.
* \param[in,out] UReducedPredictor \ref cv::Mat to store the residuals of the U component.
* \param[in,out] VReducedPredictor \ref cv::Mat to store the residuals of the V component.
*/
void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);

/**
* \brief Fazer para esta.
* 
*/
void predictor2(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);

/**
* \brief Fazer para esta.
* 
*/
void predictor3(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);

/**
* \brief Fazer para esta.
* 
*/
uint32_t getOptimalM(cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);

/**
* \brief Method to save the frequency ot the residual values of the components in files to further calculate the histograms.
* 
* It saves a file with the values to the xAxis, from -255 to 255, named xAxis.txt. \n
* For each map, it saves a file with all the frequencies of the values between -255 and 255. \n
* Namely YFrequence.txt, UFrequence.txt and VFrequence.txt. \n
* 
* \param[in] mapY \ref cv::Mat with the frequency of the residual values of Y.
* \param[in] mapU \ref cv::Mat with the frequency of the residual values of the sub-sample of U.
* \param[in] mapV \ref cv::Mat with the frequency of the residual values of the sub-sample of V.
*/
void writeMatlabVectorFiles(map<int, double> &mapY, map<int, double> &mapU, map<int, double> &mapV);

/**
* \brief Main method of the lossless encoder.
* 
* Usage: ./encoderLossless ImageName TextFile \n
*
* \param[in] ImageName \ref Image to process.
* \param[in] TextFile \ref Name of the file to save the encoded image.
*/
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Usage: ./encoderLossless ImageName TextFile" << endl;
        return -1;
    }

    Mat srcImage;

    srcImage = imread(argv[1], IMREAD_COLOR);

    if (srcImage.empty())
    {
        cout << "The image" << argv[1] << " could not be loaded." << endl;
        return -1;
    }

    //Create new matrices for YUV
    cv::Mat YComponent = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
    cv::Mat UComponent = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
    cv::Mat VComponent = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);

    int halfRows = srcImage.rows / 2;
    int halfCols = srcImage.cols / 2;

    //Correct values for odd scales
    if (srcImage.rows % 2 != 0)
    {
        halfRows += 1;
    }

    if (srcImage.cols % 2 != 0)
    {
        halfCols += 1;
    }

    cv::Mat UComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);
    cv::Mat VComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);

    convertToYUV(srcImage, YComponent, UComponent, VComponent);
    convertTo420(YComponent, UComponent, VComponent, UComponentReduced, VComponentReduced);

    //Matrices for predictor values
    cv::Mat YPredictor = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_32SC1);
    cv::Mat UReducedPredictor = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);
    cv::Mat VReducedPredictor = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);

    predictor1(YComponent, UComponentReduced, VComponentReduced, YPredictor, UReducedPredictor, VReducedPredictor);

    uint32_t m;

    //Encode Header with fixed m = 400
    Golomb encoder((string)argv[2], BitStream::bs_mode::write, 400);

    m = getOptimalM(YPredictor, UReducedPredictor, VReducedPredictor);
    std::cout << "Optimal m: " << m << std::endl;

    //encode m
    encoder.encodeNumber(m);

    //encode rows
    encoder.encodeNumber(srcImage.rows);

    //encode columns
    encoder.encodeNumber(srcImage.cols);

    //Change encoder m
    encoder.setM(m);

    //encode Y
    short *pYPred;
    for (int i = 0; i < YPredictor.rows; i++)
    {
        pYPred = YPredictor.ptr<short>(i);
        for (int j = 0; j < YPredictor.cols; j++)
        {
            encoder.encodeNumber(pYPred[j]);
        }
    }

    //encode U
    short *pUPred;
    for (int i = 0; i < UReducedPredictor.rows; i++)
    {
        pUPred = UReducedPredictor.ptr<short>(i);
        for (int j = 0; j < UReducedPredictor.cols; j++)
        {
            encoder.encodeNumber(pUPred[j]);
        }
    }

    //encode V
    short *pVPred;
    for (int i = 0; i < VReducedPredictor.rows; i++)
    {
        pVPred = VReducedPredictor.ptr<short>(i);
        for (int j = 0; j < VReducedPredictor.cols; j++)
        {
            encoder.encodeNumber(pVPred[j]);
        }
    }

    encoder.close();

    map<int, double> mapY, mapU, mapV;
    double totalY = 0, totalU = 0, totalV = 0;
    int countY = 0, countU = 0, countV = 0;

    //Frequency counting for residuals
    for (int i = -255; i < 256; i++)
    {
        countY = 0;
        countU = 0;
        countV = 0;
        for (int k = 0; k < YPredictor.rows; k++)
        {
            pYPred = YPredictor.ptr<short>(k);
            for (int j = 0; j < YPredictor.cols; j++)
            {
                if (pYPred[j] == i)
                {
                    countY++;
                    totalY++;
                }
            }
        }

        for (int k = 0; k < UReducedPredictor.rows; k++)
        {
            pUPred = UReducedPredictor.ptr<short>(k);
            pVPred = VReducedPredictor.ptr<short>(k);
            for (int j = 0; j < UReducedPredictor.cols; j++)
            {
                if (pUPred[j] == i)
                {
                    countU++;
                    totalU++;
                }

                if (pVPred[j] == i)
                {
                    countV++;
                    totalV++;
                }
            }
        }

        mapY[i] = countY;
        mapU[i] = countU;
        mapV[i] = countV;
    }

    double entropyY = 0, entropyU = 0, entropyV = 0;
    map<int, double> probMapY, probMapU, probMapV;
    double totalYProb = 0, totalUProb = 0, totalVProb = 0;

    for (int i = -255; i < 256; i++)
    {
        probMapY[i] = mapY[i] / totalY;
        probMapU[i] = mapU[i] / totalU;
        probMapV[i] = mapV[i] / totalV;

        //cout << "prob map Y: " << probMapY[i] << endl;

        if (probMapY[i] != 0)
        {
            entropyY += probMapY[i] * log2(probMapY[i]);
        }

        if (probMapU[i] != 0)
        {
            entropyU += probMapU[i] * log2(probMapU[i]);
        }

        if (probMapV[i] != 0)
        {
            entropyV += probMapV[i] * log2(probMapV[i]);
        }

        totalYProb += probMapY[i];
        totalUProb += probMapU[i];
        totalVProb += probMapV[i];
    }

    std::cout << "Y prob total"
              << ": " << totalYProb << '\n';

    std::cout << "U prob total"
              << ": " << totalUProb << '\n';

    std::cout << "V prob total"
              << ": " << totalVProb << '\n';

    std::cout << "Y entropy"
              << ": " << -entropyY << '\n';

    std::cout << "U entropy"
              << ": " << -entropyU << '\n';

    std::cout << "V entropy"
              << ": " << -entropyV << '\n';

    writeMatlabVectorFiles(mapY, mapU, mapV);

    imwrite("Y.png", YComponent);
    imwrite("U.png", UComponent);
    imwrite("V.png", VComponent);

    imwrite("UReduced.png", UComponentReduced);
    imwrite("VReduced.png", VComponentReduced);

    return 0;
}

void convertToYUV(const cv::Mat &source, cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent)
{

    int nRows = source.rows;
    int nCols = source.cols;

    /*
    Y = 0.299R + 0.587G + 0.114B
    Cb = 128 − 0.168736R − 0.331264G + 0.5B
    Cr = 128 + 0.5R − 0.418688G − 0.081312B 
    */

    uchar *pY, *pU, *pV;
    for (int i = 0; i < nRows; i++)
    {
        pY = YComponent.ptr<uchar>(i);
        pU = UComponent.ptr<uchar>(i);
        pV = VComponent.ptr<uchar>(i);
        for (int j = 0; j < nCols; j++)
        {
            //bgr
            Vec3b intensity = source.at<Vec3b>(i, j);
            pY[j] = 0.299 * intensity.val[2] + 0.587 * intensity.val[1] + 0.114 * intensity.val[0];
            pU[j] = 128 - 0.168736 * intensity.val[2] - 0.331264 * intensity.val[1] + 0.5 * intensity.val[0];
            pV[j] = 128 + 0.5 * intensity.val[2] - 0.418688 * intensity.val[1] - 0.081312 * intensity.val[0];
        }
    }
}

void convertTo420(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced)
{
    int nRows = YComponent.rows;
    int nCols = YComponent.cols;

    uchar *pU, *pV, *pUReduced, *pVReduced;
    int countRow = 0;
    int countColums = 0;

    //Removes odd rows and columns
    for (int i = 0; i < nRows; i += 2, countRow++)
    {
        countColums = 0;

        pU = UComponent.ptr<uchar>(i);
        pV = VComponent.ptr<uchar>(i);

        pUReduced = UComponentReduced.ptr<uchar>(countRow);
        pVReduced = VComponentReduced.ptr<uchar>(countRow);

        for (int j = 0; j < nCols; j += 2, countColums++)
        {
            pUReduced[countColums] = pU[j];
            pVReduced[countColums] = pV[j];
        }
    }
}

void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor)
{
    uchar *pY, *pU, *pV;
    short *pYPred, *pUPred, *pVPred;

    //Predictor for Y
    for (int i = 0; i < YComponent.rows; i++)
    {
        pY = YComponent.ptr<uchar>(i);
        pYPred = YPredictor.ptr<short>(i);
        pYPred[0] = pY[0];
        for (int j = 1; j < YComponent.cols; j++)
        {
            pYPred[j] = pY[j] - pY[j - 1];
        }
    }

    //Predictor for U and V
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        pU = UComponentReduced.ptr<uchar>(i);
        pUPred = UReducedPredictor.ptr<short>(i);
        pV = VComponentReduced.ptr<uchar>(i);
        pVPred = VReducedPredictor.ptr<short>(i);
        pUPred[0] = pU[0];
        pVPred[0] = pV[0];
        for (int j = 1; j < UComponentReduced.cols; j++)
        {
            pUPred[j] = pU[j] - pU[j - 1];

            pVPred[j] = pV[j] - pV[j - 1];
        }
    }
}

void predictor2(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor)
{
    //Predictor for Y

    //first row, Predictor = 0
    int rowLength = YComponent.rows;
    int colLength = YComponent.cols;
    for (int i = 0; i < rowLength; i++)
    {
        YPredictor.ptr<short>(0)[i] = YComponent.ptr<uchar>(0)[i];
    }
    //Remaining rows, Predictor = b
    for (int i = 1; i < rowLength; i++)
    {
        for (int j = 0; j < colLength; j++)
        {
            YPredictor.ptr<short>(i)[j] = YComponent.ptr<uchar>(i)[j] - YComponent.ptr<uchar>(i - 1)[j];
        }
    }

    //Predictor for U and V
    //first row, Predictor = 0
    rowLength = UComponentReduced.rows;
    colLength = UComponentReduced.cols;

    for (int i = 0; i < rowLength; i++)
    {
        UReducedPredictor.ptr<short>(0)[i] = UComponentReduced.ptr<uchar>(0)[i];
        VReducedPredictor.ptr<short>(0)[i] = VComponentReduced.ptr<uchar>(0)[i];
    }

    //Remaining rows, Predictor = b
    for (int i = 1; i < rowLength; i++)
    {
        for (int j = 0; j < colLength; j++)
        {
            UReducedPredictor.ptr<short>(i)[j] = UComponentReduced.ptr<uchar>(i)[j] - UComponentReduced.ptr<uchar>(i - 1)[j];
            VReducedPredictor.ptr<short>(i)[j] = VComponentReduced.ptr<uchar>(i)[j] - VComponentReduced.ptr<uchar>(i - 1)[j];
        }
    }
}

void predictor3(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor)
{
    //Predictor for Y
    //first row, Predictor = 0
    int rowLength = YComponent.rows;
    int colLength = YComponent.cols;
    for (int i = 0; i < rowLength; i++)
    {
        YPredictor.ptr<short>(0)[i] = YComponent.ptr<uchar>(0)[i];
    }

    for (int i = 1; i < rowLength; i++)
    {
        //first column predictor = 0
        YPredictor.ptr<short>(i)[0] = YComponent.ptr<uchar>(i)[0];

        //Remaining columns, Predictor = c
        for (int j = 1; j < colLength; j++)
        {
            YPredictor.ptr<short>(i)[j] = YComponent.ptr<uchar>(i)[j] - YComponent.ptr<uchar>(i - 1)[j - 1];
        }
    }

    //Predictor for U and V
    //first row, Predictor = 0
    rowLength = UComponentReduced.rows;
    colLength = UComponentReduced.cols;

    for (int i = 0; i < rowLength; i++)
    {
        UReducedPredictor.ptr<short>(0)[i] = UComponentReduced.ptr<uchar>(0)[i];
        VReducedPredictor.ptr<short>(0)[i] = VComponentReduced.ptr<uchar>(0)[i];
    }

    for (int i = 1; i < rowLength; i++)
    {
        //first column predictor = 0
        UReducedPredictor.ptr<short>(i)[0] = UComponentReduced.ptr<uchar>(i)[0];
        VReducedPredictor.ptr<short>(i)[0] = VComponentReduced.ptr<uchar>(i)[0];

        //Remaining columns, Predictor = c
        for (int j = 0; j < colLength; j++)
        {
            UReducedPredictor.ptr<short>(i)[j] = UComponentReduced.ptr<uchar>(i)[j] - UComponentReduced.ptr<uchar>(i - 1)[j - 1];
            VReducedPredictor.ptr<short>(i)[j] = VComponentReduced.ptr<uchar>(i)[j] - VComponentReduced.ptr<uchar>(i - 1)[j - 1];
        }
    }
}

uint32_t getOptimalM(cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor)
{
    uint32_t m = 0;
    std::vector<int> values;
    uint32_t size = YPredictor.rows * YPredictor.cols + UReducedPredictor.rows * UReducedPredictor.cols + VReducedPredictor.rows * VReducedPredictor.cols;

    values.resize(size);
    uint32_t valuesCount = 0;

    short *pYPred;
    for (int i = 0; i < YPredictor.rows; i++)
    {
        pYPred = YPredictor.ptr<short>(i);
        for (int j = 0; j < YPredictor.cols; j++)
        {
            values[valuesCount] = pYPred[j];
            valuesCount++;
        }
    }

    //encode U
    short *pUPred;
    for (int i = 0; i < UReducedPredictor.rows; i++)
    {
        pUPred = UReducedPredictor.ptr<short>(i);
        for (int j = 0; j < UReducedPredictor.cols; j++)
        {

            values[valuesCount] = pUPred[j];
            valuesCount++;
        }
    }

    //encode V
    short *pVPred;
    for (int i = 0; i < VReducedPredictor.rows; i++)
    {
        pVPred = VReducedPredictor.ptr<short>(i);
        for (int j = 0; j < VReducedPredictor.cols; j++)
        {
            values[valuesCount] = pVPred[j];
            valuesCount++;
        }
    }

    m = Golomb::getOtimizedM(values);

    return m;
}

void writeMatlabVectorFiles(map<int, double> &mapY, map<int, double> &mapU, map<int, double> &mapV)
{
    std::ofstream xAxisFile("../matlab/xAxis.txt");
    std::ofstream YFrequencyFile("../matlab/YFrequency.txt");
    std::ofstream UFrequencyFile("../matlab/UFrequency.txt");
    std::ofstream VFrequencyFile("../matlab/VFrequency.txt");

    for (int i = -255; i < 256; i++)
    {
        xAxisFile << i << std::endl;
        YFrequencyFile << mapY[i] << std::endl;
        UFrequencyFile << mapU[i] << std::endl;
        VFrequencyFile << mapV[i] << std::endl;
    }

    YFrequencyFile.close();
    UFrequencyFile.close();
    VFrequencyFile.close();
}