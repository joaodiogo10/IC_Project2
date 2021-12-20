#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

/** \file 
 *  Lossy encoder. \n
 *  Image transformations are saved in files. \n
 *  Frequency of residuals are written in files, under matlab/lossy/ImageName. \n
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
void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor,
                Golomb &encoder, int reduceY, int reduceU, int reduceV);

//./encoder image textFile m numberToReduceY numberToReduceU numberToReduceV
int main(int argc, char *argv[])
{
    if (argc < 7)
    {
        cout << "Usage: ./encoderLossy ImageName TextFile m numberToReduceY numberToReduceU numberToReduceV" << endl;
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

    int m, reduceY, reduceU, reduceV;
    std::istringstream myStringM((string)argv[3]);
    myStringM >> m;
    std::istringstream myStringY((string)argv[4]);
    myStringY >> reduceY;
    std::istringstream myStringU((string)argv[5]);
    myStringU >> reduceU;
    std::istringstream myStringV((string)argv[6]);
    myStringV >> reduceV;

    //Encode Header with fixed m = 400
    Golomb encoder((string)argv[2], BitStream::bs_mode::write, 400);

    //encode m
    encoder.encodeNumber(m);

    //encode rows
    encoder.encodeNumber(srcImage.rows);

    //encode columns
    encoder.encodeNumber(srcImage.cols);

    //encode reduceY
    encoder.encodeNumber(reduceY);

    //encode reduceU
    encoder.encodeNumber(reduceU);

    //encode reduceV
    encoder.encodeNumber(reduceV);

    //Change encoder m
    encoder.setM(m);

    imwrite("Y.png", YComponent);
    imwrite("U.png", UComponent);
    imwrite("V.png", VComponent);
    imwrite("UReduced.png", UComponentReduced);
    imwrite("VReduced.png", VComponentReduced);

    predictor1(YComponent, UComponentReduced, VComponentReduced, YPredictor, UReducedPredictor, VReducedPredictor, encoder, reduceY, reduceU, reduceV);

    encoder.close();

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

void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor,
                Golomb &encoder, int reduceY, int reduceU, int reduceV)
{
    uchar *pY, *pU, *pV;
    short *pYPred, *pUPred, *pVPred;

    //Predictor for Y
    for (int i = 0; i < YComponent.rows; i++)
    {
        pY = YComponent.ptr<uchar>(i);
        pYPred = YPredictor.ptr<short>(i);
        pYPred[0] = pY[0];
        encoder.encodeNumber(pYPred[0]);
        for (int j = 1; j < YComponent.cols; j++)
        {
            pYPred[j] = pY[j] - pY[j - 1];
            pYPred[j] = pYPred[j] >> reduceY;
            encoder.encodeNumber(pYPred[j]);
            pY[j] = pY[j - 1] + (pYPred[j] << reduceY);
        }
    }

    //Predictor for U
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        pU = UComponentReduced.ptr<uchar>(i);
        pUPred = UReducedPredictor.ptr<short>(i);
        pUPred[0] = pU[0];
        encoder.encodeNumber(pUPred[0]);
        for (int j = 1; j < UComponentReduced.cols; j++)
        {
            pUPred[j] = pU[j] - pU[j - 1];
            pUPred[j] = pUPred[j] >> reduceU;
            encoder.encodeNumber(pUPred[j]);
            pU[j] = pU[j - 1] + (pUPred[j] << reduceU);
        }
    }

    //Predictor for V
    for (int i = 0; i < VComponentReduced.rows; i++)
    {
        pV = VComponentReduced.ptr<uchar>(i);
        pVPred = VReducedPredictor.ptr<short>(i);
        pVPred[0] = pV[0];
        encoder.encodeNumber(pVPred[0]);
        for (int j = 1; j < VComponentReduced.cols; j++)
        {
            pVPred[j] = pV[j] - pV[j - 1];
            pVPred[j] = pVPred[j] >> reduceV;
            encoder.encodeNumber(pVPred[j]);
            pV[j] = pV[j - 1] + (pVPred[j] << reduceV);
        }
    }
}
