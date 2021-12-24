#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

/** \file 
 *  Lossless decoder. \n
 *  Decoded images are saved in files. \n
*/

void convertToRGB(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &BGR);
void revert420(cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &UComponent, cv::Mat &VComponent);

/**
* \brief Reverse method for the first of the seven linear predictors of the lossless mode of JPEG. It calculates the original values based on this mode.
* 
* It adds the previous pixel intensity to the current one. \n
* In the case of the first column it considers the previous value to be zero. \n
* \n
* Each original value is saved in the respective Mat.
* 
* \param[in] YResiduals \ref cv::Mat with the residuals of the Y component.
* \param[in] UReducedResiduals \ref cv::Mat with the residuals of the U component.
* \param[in] VReducedResiduals \ref cv::Mat with the residuals of the V component.
* \param[in,out] YComponent \ref cv::Mat to store the values of Y.
* \param[in,out] UComponentReduced \ref cv::Mat to store the sub-samples of U.
* \param[in,out] VComponentReduced \ref cv::Mat to store the sub-samples of V.
*/
void reversePredictor1(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals, cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);

/**
* \brief Reverse method for the second of the seven linear predictors of the lossless mode of JPEG. It calculates the original values based on this mode.
* 
* It adds the above pixel intensity to the current one. \n
* In the case of the first row it considers the above value to be zero. \n
* \n
* Each original value is saved in the respective Mat.
* 
* \param[in] YResiduals \ref cv::Mat with the residuals of the Y component.
* \param[in] UReducedResiduals \ref cv::Mat with the residuals of the U component.
* \param[in] VReducedResiduals \ref cv::Mat with the residuals of the V component.
* \param[in,out] YComponent \ref cv::Mat to store the values of Y.
* \param[in,out] UComponentReduced \ref cv::Mat to store the sub-samples of U.
* \param[in,out] VComponentReduced \ref cv::Mat to store the sub-samples of V.
*/
void reversePredictor2(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals, cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);

/**
* \brief Reverse method for the third of the seven linear predictors of the lossless mode of JPEG. It calculates the original values based on this mode.
* 
* It adds the top left diagonal pixel intensity to the current one. \n
* In the case of the first row and column it considers the top left diagonal value to be zero. \n
* \n
* Each original value is saved in the respective Mat.
* 
* \param[in] YResiduals \ref cv::Mat with the residuals of the Y component.
* \param[in] UReducedResiduals \ref cv::Mat with the residuals of the U component.
* \param[in] VReducedResiduals \ref cv::Mat with the residuals of the V component.
* \param[in,out] YComponent \ref cv::Mat to store the values of Y.
* \param[in,out] UComponentReduced \ref cv::Mat to store the sub-samples of U.
* \param[in,out] VComponentReduced \ref cv::Mat to store the sub-samples of V.
*/
void reversePredictor3(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals, cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);

/**
* \brief Main method of the lossless decoder.
* 
* Usage: ./decoderLossless EncodedFile \n
*
* \param[in] EncodedFile \ref Name of the file to where the encoded image is saved.
*/
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./decoderLossless EncodedFile" << endl;
        return -1;
    }

    int32_t m, rows, cols;

    //Decode header with fixed m = 400
    Golomb decoder((string)argv[1], BitStream::bs_mode::read, 400);

    m = decoder.decodeNumber();
    rows = decoder.decodeNumber();
    cols = decoder.decodeNumber();

    int halfRows = rows / 2;
    int halfCols = cols / 2;

    //Correct values for odd scales
    if (rows % 2 != 0)
    {
        halfRows += 1;
    }

    if (cols % 2 != 0)
    {
        halfCols += 1;
    }

    //Change decoder m
    decoder.setM(m);

    //Matrices for predictor values
    cv::Mat YResiduals = cv::Mat::zeros(rows, cols, CV_32SC1);
    cv::Mat UReducedResiduals = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);
    cv::Mat VReducedResiduals = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);

    //Decode Y
    for (int i = 0; i < YResiduals.rows; i++)
    {
        for (int j = 0; j < YResiduals.cols; j++)
        {
            YResiduals.ptr<short>(i)[j] = decoder.decodeNumber();
        }
    }

    //Decode U
    for (int i = 0; i < UReducedResiduals.rows; i++)
    {
        for (int j = 0; j < UReducedResiduals.cols; j++)
        {
            UReducedResiduals.ptr<short>(i)[j] = decoder.decodeNumber();
        }
    }

    //Decode V
    for (int i = 0; i < VReducedResiduals.rows; i++)
    {
        for (int j = 0; j < VReducedResiduals.cols; j++)
        {
            VReducedResiduals.ptr<short>(i)[j] = decoder.decodeNumber();
        }
    }

    decoder.close();

    cv::Mat YComponent = cv::Mat::zeros(rows, cols, CV_8UC1);
    cv::Mat UComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);
    cv::Mat VComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);

    reversePredictor1(YResiduals, UReducedResiduals, VReducedResiduals, YComponent, UComponentReduced, VComponentReduced);

    imwrite("YDecoded.png", YComponent);

    imwrite("UReducedDecoded.png", UComponentReduced);
    imwrite("VReducedDecoded.png", VComponentReduced);

    cv::Mat UComponent = cv::Mat::zeros(rows, cols, CV_8UC1);
    cv::Mat VComponent = cv::Mat::zeros(rows, cols, CV_8UC1);

    revert420(UComponentReduced, VComponentReduced, UComponent, VComponent);

    cv::Mat BGR;

    convertToRGB(YComponent, UComponent, VComponent, BGR);

    cout << BGR << endl;

    imwrite("RGBDecoded.png", BGR);

    return 0;
}

void convertToRGB(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &BGR)
{

    /*
    R = 1.164 * Y             + 1.596 * V;
    G = 1.164 * Y - 0.392 * U - 0.813 * V;
    B = 1.164 * Y + 2.017 * U;
    */

    Mat channelR(YComponent.rows, YComponent.cols, CV_8UC1);
    Mat channelG(YComponent.rows, YComponent.cols, CV_8UC1);
    Mat channelB(YComponent.rows, YComponent.cols, CV_8UC1);

    uchar *pY, *pU, *pV;
    for (int i = 0; i < YComponent.rows; i++)
    {
        pY = YComponent.ptr<uchar>(i);
        pU = UComponent.ptr<uchar>(i);
        pV = VComponent.ptr<uchar>(i);
        for (int j = 0; j < YComponent.cols; j++)
        {
            //bgr
            channelR.ptr<uchar>(i)[j] = pY[j] + 1.403 * pV[j];
            channelG.ptr<uchar>(i)[j] = pY[j] - 0.344 * pU[j] - 0.714 * pV[j];
            channelB.ptr<uchar>(i)[j] = pY[j] + 1.770 * pU[j];
        }
    }

    std::vector<Mat> channels{channelB, channelG, channelR};

    // Create the output matrix
    merge(channels, BGR);
}

void revert420(cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &UComponent, cv::Mat &VComponent)
{
    int nRows = UComponentReduced.rows;
    int nCols = UComponentReduced.cols;

    int countRow = 0;
    int countColums = 0;

    for (int i = 0; i < nRows; i++, countRow += 2)
    {
        countColums = 0;

        for (int j = 0; j < nCols; j++, countColums += 2)
        {
            UComponent.ptr<uchar>(countRow)[countColums] = UComponentReduced.ptr<uchar>(i)[j];
            UComponent.ptr<uchar>(countRow)[countColums + 1] = UComponentReduced.ptr<uchar>(i)[j];
            UComponent.ptr<uchar>(countRow + 1)[countColums] = UComponentReduced.ptr<uchar>(i)[j];
            UComponent.ptr<uchar>(countRow + 1)[countColums + 1] = UComponentReduced.ptr<uchar>(i)[j];

            VComponent.ptr<uchar>(countRow)[countColums] = VComponentReduced.ptr<uchar>(i)[j];
            VComponent.ptr<uchar>(countRow)[countColums + 1] = VComponentReduced.ptr<uchar>(i)[j];
            VComponent.ptr<uchar>(countRow + 1)[countColums] = VComponentReduced.ptr<uchar>(i)[j];
            VComponent.ptr<uchar>(countRow + 1)[countColums + 1] = VComponentReduced.ptr<uchar>(i)[j];
        }
    }
}

void reversePredictor1(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals, cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced)
{
    //Reverse predictor for Y
    for (int i = 0; i < YComponent.rows; i++)
    {
        YComponent.ptr<uchar>(i)[0] = YResiduals.ptr<short>(i)[0];
        for (int j = 1; j < YComponent.cols; j++)
        {
            YComponent.ptr<uchar>(i)[j] = YResiduals.ptr<short>(i)[j] + YComponent.ptr<uchar>(i)[j - 1];
        }
    }

    //Reverse predictor for U and V
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        UComponentReduced.ptr<uchar>(i)[0] = UReducedResiduals.ptr<short>(i)[0];
        VComponentReduced.ptr<uchar>(i)[0] = VReducedResiduals.ptr<short>(i)[0];
        for (int j = 1; j < UComponentReduced.cols; j++)
        {
            UComponentReduced.ptr<uchar>(i)[j] = UReducedResiduals.ptr<short>(i)[j] + UComponentReduced.ptr<uchar>(i)[j - 1];
            VComponentReduced.ptr<uchar>(i)[j] = VReducedResiduals.ptr<short>(i)[j] + VComponentReduced.ptr<uchar>(i)[j - 1];
        }
    }
}

void reversePredictor2(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals, cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced)
{
    //reverse Predictor for Y
    //first row, Predictor = 0
    for (int i = 0; i < YComponent.rows; i++)
    {
        YComponent.ptr<uchar>(0)[i] = YResiduals.ptr<short>(0)[i];
    }

    //Remaining rows, Predictor = b
    for (int i = 1; i < YComponent.rows; i++)
    {
        for (int j = 0; j < YComponent.cols; j++)
        {
            YComponent.ptr<uchar>(i)[j] = YResiduals.ptr<short>(i)[j] + YComponent.ptr<uchar>(i - 1)[j];
        }
    }

    //reverse Predictor for U and V
    //first row, Predictor = 0
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        UComponentReduced.ptr<uchar>(0)[i] = UReducedResiduals.ptr<short>(0)[i];
        VComponentReduced.ptr<uchar>(0)[i] = VReducedResiduals.ptr<short>(0)[i];
    }

    //Remaining rows, Predictor = b
    for (int i = 1; i < UComponentReduced.rows; i++)
    {
        for (int j = 0; j < UComponentReduced.cols; j++)
        {
            UComponentReduced.ptr<uchar>(i)[j] = UReducedResiduals.ptr<short>(i)[j] + UComponentReduced.ptr<uchar>(i - 1)[j];
            VComponentReduced.ptr<uchar>(i)[j] = VReducedResiduals.ptr<short>(i)[j] + VComponentReduced.ptr<uchar>(i - 1)[j];
        }
    }
}

void reversePredictor3(cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals, cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced)
{
    //reverse Predictor for Y
    //first row, Predictor = 0
    for (int i = 0; i < YComponent.rows; i++)
    {
        YComponent.ptr<uchar>(0)[i] = YResiduals.ptr<short>(0)[i];
    }

    for (int i = 1; i < YComponent.rows; i++)
    {
        //first column predictor = 0
        YComponent.ptr<uchar>(i)[0] = YResiduals.ptr<short>(i)[0];

        //Remaining columns, Predictor = c
        for (int j = 1; j < YComponent.cols; j++)
        {
            YComponent.ptr<uchar>(i)[j] = YResiduals.ptr<short>(i)[j] + YComponent.ptr<uchar>(i - 1)[j - 1];
        }
    }

    //reverse Predictor for U and V
    //first row, Predictor = 0
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        UComponentReduced.ptr<uchar>(0)[i] = UReducedResiduals.ptr<short>(0)[i];
        VComponentReduced.ptr<uchar>(0)[i] = VReducedResiduals.ptr<short>(0)[i];
    }

    for (int i = 1; i < UComponentReduced.rows; i++)
    {
        //first column predictor = 0
        UComponentReduced.ptr<uchar>(i)[0] = UReducedResiduals.ptr<short>(i)[0];
        VComponentReduced.ptr<uchar>(i)[0] = VReducedResiduals.ptr<short>(i)[0];

        //Remaining columns, Predictor = c
        for (int j = 0; j < UComponentReduced.cols; j++)
        {
            UComponentReduced.ptr<uchar>(i)[j] = UReducedResiduals.ptr<short>(i)[j] + UComponentReduced.ptr<uchar>(i - 1)[j - 1];
            VComponentReduced.ptr<uchar>(i)[j] = VReducedResiduals.ptr<short>(i)[j] + VComponentReduced.ptr<uchar>(i - 1)[j - 1];
        }
    }
}