#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

/** \file 
 *  Lossy decoder. \n
 *  Decoded images are saved in files. \n
*/

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
* \brief Main method of the lossy decoder.
* 
* Usage: ./decoderLossy EncodedFile \n
* 
* It creates YDecoded.png, UReducedDecoded.png and VReducedDecoded.png.
*
* \param[in] EncodedFile Name of the file to where the encoded image is saved.
*/
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./decoderLossy EncodedFile" << endl;
        return -1;
    }

    int32_t m, rows, cols, reduceY, reduceU, reduceV, predictor;

    //Decode header with fixed m = 400
    Golomb decoder((string)argv[1], BitStream::bs_mode::read, 400);

    m = decoder.decodeNumber();
    rows = decoder.decodeNumber();
    cols = decoder.decodeNumber();
    predictor = decoder.decodeNumber();
    reduceY = decoder.decodeNumber();
    reduceU = decoder.decodeNumber();
    reduceV = decoder.decodeNumber();

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

    //decode Y
    short *pYRes;
    for (int i = 0; i < YResiduals.rows; i++)
    {
        pYRes = YResiduals.ptr<short>(i);
        pYRes[0] = decoder.decodeNumber();

        for (int j = 1; j < YResiduals.cols; j++)
        {
            pYRes[j] = decoder.decodeNumber();
            pYRes[j] = pYRes[j] << reduceY;
        }
    }

    //decode U
    short *pURes;
    for (int i = 0; i < UReducedResiduals.rows; i++)
    {
        pURes = UReducedResiduals.ptr<short>(i);
        pURes[0] = decoder.decodeNumber();

        for (int j = 1; j < UReducedResiduals.cols; j++)
        {
            pURes[j] = decoder.decodeNumber();
            pURes[j] = pURes[j] << reduceU;
        }
    }

    //decode V
    short *pVRes;
    for (int i = 0; i < VReducedResiduals.rows; i++)
    {
        pVRes = VReducedResiduals.ptr<short>(i);
        pVRes[0] = decoder.decodeNumber();

        for (int j = 1; j < VReducedResiduals.cols; j++)
        {
            pVRes[j] = decoder.decodeNumber();
            pVRes[j] = pVRes[j] << reduceV;
        }
    }

    decoder.close();

    cv::Mat YComponent = cv::Mat::zeros(rows, cols, CV_8UC1);
    cv::Mat UComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);
    cv::Mat VComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);

    switch (predictor)
    {
    case 1:
        reversePredictor1(YResiduals, UReducedResiduals, VReducedResiduals, YComponent, UComponentReduced, VComponentReduced);
        break;
    case 2:
        reversePredictor2(YResiduals, UReducedResiduals, VReducedResiduals, YComponent, UComponentReduced, VComponentReduced);
        break;
    case 3:
        reversePredictor3(YResiduals, UReducedResiduals, VReducedResiduals, YComponent, UComponentReduced, VComponentReduced);
        break;
    default:
        break;
    }

    imwrite("YDecoded.png", YComponent);

    imwrite("UReducedDecoded.png", UComponentReduced);
    imwrite("VReducedDecoded.png", VComponentReduced);

    return 0;
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