#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

//Agr da malloc para a imagem house.ppm na funçao yuv

void convertToYUV(const cv::Mat &source, cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent);
void convertTo420(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);
void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);

//./encoder image textFile m
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cout << "Not enough parameters" << endl;
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

    int m;
    std::istringstream myStringM((string)argv[3]);
    myStringM >> m;

    //Encode Header with fixed m = 400
    Golomb encoder((string)argv[2], BitStream::bs_mode::write, 400);

    //encode m
    encoder.encodeNumber(m);
    cout << "m: " << m << endl;

    //encode rows
    encoder.encodeNumber(srcImage.rows);
    cout << "rows " << srcImage.rows << endl;

    //encode columns
    encoder.encodeNumber(srcImage.cols);
    cout << "colmns " << srcImage.cols << endl;

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

    //tenho de fzr fillWithPadding ???

    encoder.close();

    imwrite("Y.jpeg", YComponent);
    imwrite("U.jpeg", UComponent);
    imwrite("V.jpeg", VComponent);

    imwrite("UReduced.jpeg", UComponentReduced);
    imwrite("VReduced.jpeg", VComponentReduced);

    return 0;
}

void convertToYUV(const cv::Mat &source, cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent)
{
    int channels = source.channels();

    int nRows = source.rows;
    int nCols = source.cols * channels;

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
    int channels = YComponent.channels();

    int nRows = YComponent.rows;
    int nCols = YComponent.cols * channels;

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
    uchar *pY, *pU, *pV, previous, secondPrevious;
    short *pYPred, *pUPred, *pVPred, r;
    previous = 0;
    int count = 0;

    //Predictor for Y
    for (int i = 0; i < YComponent.rows; i++)
    {

        pY = YComponent.ptr<uchar>(i);
        pYPred = YPredictor.ptr<short>(i);
        for (int j = 0; j < YComponent.cols; j++, count++)
        {

            r = pY[j] - previous;
            previous = pY[j];
            pYPred[j] = r;
        }
    }

    previous = 0;
    secondPrevious = 0;

    //Predictor for U and V
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        pU = UComponentReduced.ptr<uchar>(i);
        pUPred = UReducedPredictor.ptr<short>(i);
        pV = VComponentReduced.ptr<uchar>(i);
        pVPred = VReducedPredictor.ptr<short>(i);
        for (int j = 0; j < UComponentReduced.cols; j++)
        {
            r = pU[j] - previous;
            previous = pU[j];
            pUPred[j] = r;

            r = pV[j] - secondPrevious;
            secondPrevious = pV[j];
            pVPred[j] = r;
        }
    }
}
