#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

/** \file 
 *  Lossless encoder. \n
 *  Image transformations are saved in files. \n
 *  Frequency of residuals are written in files. \n
 *  File with the encoded information is written. \n
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
* Removes odd rows and columns of the \p UComponent and \p VComponent chrominance components. \n
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
* It subtracts the previous pixel intensity from the current one. \n
* In the case of the first column it considers the previous value to be zero. \n
* \n
* Each residual value is saved in the respective Mat.
* 
* \param[in] YComponent \ref cv::Mat with the values of Y.
* \param[in] UComponentReduced \ref cv::Mat with the sub-samples of U.
* \param[in] VComponentReduced \ref cv::Mat with the sub-samples of V.
* \param[in,out] YResiduals \ref cv::Mat to store the residuals of the Y component.
* \param[in,out] UReducedResiduals \ref cv::Mat to store the residuals of the U component.
* \param[in,out] VReducedResiduals \ref cv::Mat to store the residuals of the V component.
*/
void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals);

/**
* \brief Second of the seven linear predictors of the lossless mode of JPEG. It calculates the residuals based on this mode.
* 
* It subtracts the above pixel intensity from the current one. \n
* In the case of the first row it considers the above value to be zero. \n
* \n
* Each residual value is saved in the respective Mat.
* 
* \param[in] YComponent \ref cv::Mat with the values of Y.
* \param[in] UComponentReduced \ref cv::Mat with the sub-samples of U.
* \param[in] VComponentReduced \ref cv::Mat with the sub-samples of V.
* \param[in,out] YResiduals \ref cv::Mat to store the residuals of the Y component.
* \param[in,out] UReducedResiduals \ref cv::Mat to store the residuals of the U component.
* \param[in,out] VReducedResiduals \ref cv::Mat to store the residuals of the V component.
*/
void predictor2(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals);

/**
* \brief Third of the seven linear predictors of the lossless mode of JPEG. It calculates the residuals based on this mode.
* 
* It subtracts the top left diagonal pixel intensity from the current one. \n
* In the case of the first row and first column it considers the top left diagonal value to be zero. \n
* \n
* Each residual value is saved in the respective Mat.
* 
* \param[in] YComponent \ref cv::Mat with the values of Y.
* \param[in] UComponentReduced \ref cv::Mat with the sub-samples of U.
* \param[in] VComponentReduced \ref cv::Mat with the sub-samples of V.
* \param[in,out] YResiduals \ref cv::Mat to store the residuals of the Y component.
* \param[in,out] UReducedResiduals \ref cv::Mat to store the residuals of the U component.
* \param[in,out] VReducedResiduals \ref cv::Mat to store the residuals of the V component.
*/
void predictor3(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals);

/**
* \brief Sixth of the seven linear predictors of the lossless mode of JPEG. It calculates the residuals based on this mode.
* 
* It follows the expression: x = y - (b + (a - c)/2) \n
* \n
* Each residual value is saved in the respective Mat.
* 
* \param[in] YComponent \ref cv::Mat with the values of Y.
* \param[in] UComponentReduced \ref cv::Mat with the sub-samples of U.
* \param[in] VComponentReduced \ref cv::Mat with the sub-samples of V.
* \param[in,out] YResiduals \ref cv::Mat to store the residuals of the Y component.
* \param[in,out] UReducedResiduals \ref cv::Mat to store the residuals of the U component.
* \param[in,out] VReducedResiduals \ref cv::Mat to store the residuals of the V component.
*/
void predictor6(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals);

/**
* \brief Method that returns "optimal" M golomb parameter for the value distribution given by residuals for YUV components.
* 
* \param YResiduals \ref cv::Mat with the residuals of Y.
* \param UReducedResiduals \ref cv::Mat with the residuals of U.
* \param VReducedResiduals \ref cv::Mat with the residuals of V.
*/
uint32_t getOptimalM(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals);

/**
* \brief Method to save the frequency of the residual values of the components in files to further calculate the histograms.
* 
* It saves a file with the values to the xAxis, from -255 to 255, named xAxis.txt. \n
* For each map, it saves a file with all the frequencies of the values between -255 and 255. \n
* Namely YFrequence.txt, UFrequence.txt and VFrequence.txt. \n
* 
* \param[in] mapY Map with the frequency of the residual values of Y.
* \param[in] mapU Map with the frequency of the residual values of the sub-sample of U.
* \param[in] mapV Map with the frequency of the residual values of the sub-sample of V.
*/
void writeMatlabVectorFiles(map<int, double> &mapY, map<int, double> &mapU, map<int, double> &mapV);

/**
* \brief Main method of the lossless encoder.
* 
* Usage: ./encoderLossless ImageName EncodedFile \n
*
* It creates Y.png, U.png, V.png, UReduced.png, VReduced.png, xAxis.txt, YFrequency.txt, UFrequency.txt and VFrequency.txt.
*
* \param[in] ImageName Image to process.
* \param[in] EncodedFile Name of the file to save the encoded image.
* \param[in] PredictorMode Number of the desired predictor mode. 1 - a; 2 - b; 3 - c; 6 - b + (a-c)/2
*/
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cout << "Usage: ./encoderLossless ImageName EncodedFile PredictorMode" << endl;
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

    //Matrices for residual values
    cv::Mat YResiduals = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_32SC1);
    cv::Mat UReducedResiduals = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);
    cv::Mat VReducedResiduals = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);

    int predictor;
    std::istringstream myStringPredictor((string)argv[3]);
    myStringPredictor >> predictor;

    switch (predictor)
    {
    case 1:
        predictor1(YComponent, UComponentReduced, VComponentReduced, YResiduals, UReducedResiduals, VReducedResiduals);
        break;
    case 2:
        predictor2(YComponent, UComponentReduced, VComponentReduced, YResiduals, UReducedResiduals, VReducedResiduals);
        break;
    case 3:
        predictor3(YComponent, UComponentReduced, VComponentReduced, YResiduals, UReducedResiduals, VReducedResiduals);
        break;
    case 6:
        predictor6(YComponent, UComponentReduced, VComponentReduced, YResiduals, UReducedResiduals, VReducedResiduals);
        break;
    default:
        break;
    }

    //Encode Header with fixed m = 400
    Golomb encoder((string)argv[2], BitStream::bs_mode::write, 400);

    uint32_t m = getOptimalM(YResiduals, UReducedResiduals, VReducedResiduals);
    std::cout << "Optimal m: " << m << std::endl;

    //encode m
    encoder.encodeNumber(m);

    //encode rows
    encoder.encodeNumber(srcImage.rows);

    //encode columns
    encoder.encodeNumber(srcImage.cols);

    //encode predictor mode
    encoder.encodeNumber(predictor);

    //Change encoder m
    encoder.setM(m);

    //encode Y
    for (int i = 0; i < YResiduals.rows; i++)
    {
        for (int j = 0; j < YResiduals.cols; j++)
        {
            encoder.encodeNumber(YResiduals.ptr<short>(i)[j]);
        }
    }

    //encode U
    for (int i = 0; i < UReducedResiduals.rows; i++)
    {
        for (int j = 0; j < UReducedResiduals.cols; j++)
        {
            encoder.encodeNumber(UReducedResiduals.ptr<short>(i)[j]);
        }
    }

    //encode V
    for (int i = 0; i < VReducedResiduals.rows; i++)
    {
        for (int j = 0; j < VReducedResiduals.cols; j++)
        {
            encoder.encodeNumber(VReducedResiduals.ptr<short>(i)[j]);
        }
    }

    encoder.close();

    map<int, double> mapY, mapU, mapV;
    double totalY = 0, totalU = 0, totalV = 0;

    for (int i = -255; i < 256; i++)
    {
        mapY[i] = 0;
        mapU[i] = 0;
        mapV[i] = 0;
    }

    //Frequency counting for residuals
    for (int i = 0; i < YResiduals.rows; i++)
    {
        for (int j = 0; j < YResiduals.cols; j++)
        {
            mapY[YResiduals.ptr<short>(i)[j]] += 1;
            totalY++;
        }
    }

    for (int i = 0; i < UReducedResiduals.rows; i++)
    {
        for (int j = 0; j < UReducedResiduals.cols; j++)
        {
            mapU[UReducedResiduals.ptr<short>(i)[j]] += 1;
            mapV[VReducedResiduals.ptr<short>(i)[j]] += 1;
            totalU++;
            totalV++;
        }
    }

    double entropyY = 0, entropyU = 0, entropyV = 0;
    map<int, double> probMapY, probMapU, probMapV;

    for (int i = -255; i < 256; i++)
    {
        probMapY[i] = mapY[i] / totalY;
        probMapU[i] = mapU[i] / totalU;
        probMapV[i] = mapV[i] / totalV;

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
    }

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

/* void convertTo420(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced)
{
    int nRows = YComponent.rows;
    int nCols = YComponent.cols;

    int countRow = 0;
    int countColums = 0;

    //Removes odd rows and columns
    for (int i = 1; i < nRows; i += 2, countRow++)
    {
        countColums = 0;
        for (int j = 1; j < nCols; j += 2, countColums++)
        {
            uchar meanU = (UComponent.ptr<uchar>(i-1)[j-1] + UComponent.ptr<uchar>(i-1)[j] + UComponent.ptr<uchar>(i)[j-1] + UComponent.ptr<uchar>(i)[j]) / 4;
            uchar meanV = (VComponent.ptr<uchar>(i-1)[j-1] + VComponent.ptr<uchar>(i-1)[j] + VComponent.ptr<uchar>(i)[j-1] + VComponent.ptr<uchar>(i)[j]) / 4;
            UComponentReduced.ptr<uchar>(countRow)[countColums] = meanU;
            VComponentReduced.ptr<uchar>(countRow)[countColums] = meanV;
        }
    }
} */
void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals)
{
    //Residuals for Y
    for (int i = 0; i < YComponent.rows; i++)
    {
        YResiduals.ptr<short>(i)[0] = YComponent.ptr<uchar>(i)[0];
        for (int j = 1; j < YComponent.cols; j++)
        {
            YResiduals.ptr<short>(i)[j] = YComponent.ptr<uchar>(i)[j] - YComponent.ptr<uchar>(i)[j - 1];
        }
    }

    //Predictor for U and V
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        UReducedResiduals.ptr<short>(i)[0] = UComponentReduced.ptr<uchar>(i)[0];
        VReducedResiduals.ptr<short>(i)[0] = VComponentReduced.ptr<uchar>(i)[0];
        for (int j = 1; j < UComponentReduced.cols; j++)
        {
            UReducedResiduals.ptr<short>(i)[j] = UComponentReduced.ptr<uchar>(i)[j] - UComponentReduced.ptr<uchar>(i)[j - 1];
            VReducedResiduals.ptr<short>(i)[j] = VComponentReduced.ptr<uchar>(i)[j] - VComponentReduced.ptr<uchar>(i)[j - 1];
        }
    }
}

void predictor2(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals)
{
    //Residuals for Y
    //first row, Predictor = 0
    for (int j = 0; j < YComponent.cols; j++)
    {
        YResiduals.ptr<short>(0)[j] = YComponent.ptr<uchar>(0)[j];
    }
    //Remaining rows, Predictor = b
    for (int i = 1; i < YComponent.rows; i++)
    {
        for (int j = 0; j < YComponent.cols; j++)
        {
            YResiduals.ptr<short>(i)[j] = YComponent.ptr<uchar>(i)[j] - YComponent.ptr<uchar>(i - 1)[j];
        }
    }

    //Residuals for U and V
    //first row, Predictor = 0
    for (int j = 0; j < UComponentReduced.cols; j++)
    {
        UReducedResiduals.ptr<short>(0)[j] = UComponentReduced.ptr<uchar>(0)[j];
        VReducedResiduals.ptr<short>(0)[j] = VComponentReduced.ptr<uchar>(0)[j];
    }

    //Remaining rows, Predictor = b
    for (int i = 1; i < UComponentReduced.rows; i++)
    {
        for (int j = 0; j < UComponentReduced.cols; j++)
        {
            UReducedResiduals.ptr<short>(i)[j] = UComponentReduced.ptr<uchar>(i)[j] - UComponentReduced.ptr<uchar>(i - 1)[j];
            VReducedResiduals.ptr<short>(i)[j] = VComponentReduced.ptr<uchar>(i)[j] - VComponentReduced.ptr<uchar>(i - 1)[j];
        }
    }
}

void predictor3(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals)
{
    //Residuals for Y
    //first row, Predictor = 0
    for (int j = 0; j < YComponent.cols; j++)
    {
        YResiduals.ptr<short>(0)[j] = YComponent.ptr<uchar>(0)[j];
    }

    for (int i = 1; i < YComponent.rows; i++)
    {
        //first column predictor = 0
        YResiduals.ptr<short>(i)[0] = YComponent.ptr<uchar>(i)[0];

        //Remaining columns, Predictor = c
        for (int j = 1; j < YComponent.cols; j++)
        {
            YResiduals.ptr<short>(i)[j] = YComponent.ptr<uchar>(i)[j] - YComponent.ptr<uchar>(i - 1)[j - 1];
        }
    }

    //Residuals for U and V
    //first row, Predictor = 0
    for (int j = 0; j < UComponentReduced.cols; j++)
    {
        UReducedResiduals.ptr<short>(0)[j] = UComponentReduced.ptr<uchar>(0)[j];
        VReducedResiduals.ptr<short>(0)[j] = VComponentReduced.ptr<uchar>(0)[j];
    }

    for (int i = 1; i < UComponentReduced.rows; i++)
    {
        //first column predictor = 0
        UReducedResiduals.ptr<short>(i)[0] = UComponentReduced.ptr<uchar>(i)[0];
        VReducedResiduals.ptr<short>(i)[0] = VComponentReduced.ptr<uchar>(i)[0];

        //Remaining columns, Predictor = c
        for (int j = 0; j < UComponentReduced.cols; j++)
        {
            UReducedResiduals.ptr<short>(i)[j] = UComponentReduced.ptr<uchar>(i)[j] - UComponentReduced.ptr<uchar>(i - 1)[j - 1];
            VReducedResiduals.ptr<short>(i)[j] = VComponentReduced.ptr<uchar>(i)[j] - VComponentReduced.ptr<uchar>(i - 1)[j - 1];
        }
    }
}

void predictor6(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals)
{
    //Residuals for Y
    //a = 0, b = 0, c = 0
    YResiduals.ptr<short>(0)[0] = YComponent.ptr<uchar>(0)[0];

    //First column, a = 0, c = 0
    for (int i = 1; i < YComponent.rows; i++)
    {
        YResiduals.ptr<short>(i)[0] = YComponent.ptr<uchar>(i)[0] - YComponent.ptr<uchar>(i - 1)[0];
    }

    //First row, b = 0, c = 0
    for (int j = 1; j < YComponent.cols; j++)
    {
        YResiduals.ptr<short>(0)[j] = YComponent.ptr<uchar>(0)[j] - YComponent.ptr<uchar>(0)[j - 1];
    }

    //Remaining
    for (int i = 1; i < YComponent.rows; i++)
    {
        for (int j = 1; j < YComponent.cols; j++)
        {
            YResiduals.ptr<short>(i)[j] = YComponent.ptr<uchar>(i)[j] - (YComponent.ptr<uchar>(i - 1)[j] + (YComponent.ptr<uchar>(i)[j - 1] - YComponent.ptr<uchar>(i - 1)[j - 1]) / 2);
        }
    }

    //Residuals for U and V
    //a = 0, b = 0, c = 0
    UReducedResiduals.ptr<short>(0)[0] = UComponentReduced.ptr<uchar>(0)[0];
    VReducedResiduals.ptr<short>(0)[0] = VComponentReduced.ptr<uchar>(0)[0];

    //First column, a = 0, c = 0
    for (int i = 1; i < UComponentReduced.rows; i++)
    {
        UReducedResiduals.ptr<short>(i)[0] = UComponentReduced.ptr<uchar>(i)[0] - UComponentReduced.ptr<uchar>(i - 1)[0];
        VReducedResiduals.ptr<short>(i)[0] = VComponentReduced.ptr<uchar>(i)[0] - VComponentReduced.ptr<uchar>(i - 1)[0];
    }

    //First row, b = 0, c = 0
    for (int j = 1; j < UComponentReduced.cols; j++)
    {
        UReducedResiduals.ptr<short>(0)[j] = UComponentReduced.ptr<uchar>(0)[j] - UComponentReduced.ptr<uchar>(0)[j - 1];
        VReducedResiduals.ptr<short>(0)[j] = VComponentReduced.ptr<uchar>(0)[j] - VComponentReduced.ptr<uchar>(0)[j - 1];
    }

    //Remaining
    for (int i = 1; i < UComponentReduced.rows; i++)
    {
        for (int j = 1; j < UComponentReduced.cols; j++)
        {
            UReducedResiduals.ptr<short>(i)[j] = UComponentReduced.ptr<uchar>(i)[j] - (UComponentReduced.ptr<uchar>(i - 1)[j] + (UComponentReduced.ptr<uchar>(i)[j - 1] - UComponentReduced.ptr<uchar>(i - 1)[j - 1]) / 2);
            VReducedResiduals.ptr<short>(i)[j] = VComponentReduced.ptr<uchar>(i)[j] - (VComponentReduced.ptr<uchar>(i - 1)[j] + (VComponentReduced.ptr<uchar>(i)[j - 1] - VComponentReduced.ptr<uchar>(i - 1)[j - 1]) / 2);
        }
    }
}

uint32_t getOptimalM(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals)
{
    uint32_t m = 0;
    std::vector<int> values;
    uint32_t size = YResiduals.rows * YResiduals.cols + UReducedResiduals.rows * UReducedResiduals.cols + VReducedResiduals.rows * VReducedResiduals.cols;

    values.resize(size);
    uint32_t valuesCount = 0;

    for (int i = 0; i < YResiduals.rows; i++)
    {
        for (int j = 0; j < YResiduals.cols; j++)
        {
            values[valuesCount] = YResiduals.ptr<short>(i)[j];
            valuesCount++;
        }
    }

    for (int i = 0; i < UReducedResiduals.rows; i++)
    {
        for (int j = 0; j < UReducedResiduals.cols; j++)
        {
            values[valuesCount] = UReducedResiduals.ptr<short>(i)[j];
            valuesCount++;
        }
    }

    for (int i = 0; i < VReducedResiduals.rows; i++)
    {
        for (int j = 0; j < VReducedResiduals.cols; j++)
        {
            values[valuesCount] = VReducedResiduals.ptr<short>(i)[j];
            valuesCount++;
        }
    }

    m = Golomb::getOtimizedM(values);

    return m;
}

void writeMatlabVectorFiles(map<int, double> &mapY, map<int, double> &mapU, map<int, double> &mapV)
{
    std::ofstream xAxisFile("xAxis.txt");
    std::ofstream YFrequencyFile("YFrequency.txt");
    std::ofstream UFrequencyFile("UFrequency.txt");
    std::ofstream VFrequencyFile("VFrequency.txt");

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