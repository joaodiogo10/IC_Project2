#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

void reversePredictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);

//./decoder textFile
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./decoder TextFile" << endl;
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
    cv::Mat YPredictor = cv::Mat::zeros(rows, cols, CV_32SC1);
    cv::Mat UReducedPredictor = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);
    cv::Mat VReducedPredictor = cv::Mat::zeros(halfRows, halfCols, CV_32SC1);

    //encode Y
    short *pYPred;
    for (int i = 0; i < YPredictor.rows; i++)
    {
        pYPred = YPredictor.ptr<short>(i);
        for (int j = 0; j < YPredictor.cols; j++)
        {
            pYPred[j] = decoder.decodeNumber();
        }
    }

    //encode U
    short *pUPred;
    for (int i = 0; i < UReducedPredictor.rows; i++)
    {
        pUPred = UReducedPredictor.ptr<short>(i);
        for (int j = 0; j < UReducedPredictor.cols; j++)
        {
            pUPred[j] = decoder.decodeNumber();
        }
    }

    //encode V
    short *pVPred;
    for (int i = 0; i < VReducedPredictor.rows; i++)
    {
        pVPred = VReducedPredictor.ptr<short>(i);
        for (int j = 0; j < VReducedPredictor.cols; j++)
        {
            pVPred[j] = decoder.decodeNumber();
        }
    }

    decoder.close();

    cv::Mat YComponent = cv::Mat::zeros(rows, cols, CV_8UC1);
    cv::Mat UComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);
    cv::Mat VComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);

    reversePredictor1(YComponent, UComponentReduced, VComponentReduced, YPredictor, UReducedPredictor, VReducedPredictor);

    imwrite("YDecoded.jpeg", YComponent);

    imwrite("UReducedDecoded.jpeg", UComponentReduced);
    imwrite("VReducedDecoded.jpeg", VComponentReduced);

    return 0;
}

void reversePredictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor)
{

    uchar *pY, *pU, *pV, previous, secondPrevious;
    short *pYPred, *pUPred, *pVPred;
    previous = 0;

    //Reverse predictor for Y
    for (int i = 0; i < YComponent.rows; i++)
    {
        pY = YComponent.ptr<uchar>(i);
        pYPred = YPredictor.ptr<short>(i);
        for (int j = 0; j < YComponent.cols; j++)
        {
            pY[j] = pYPred[j] + previous;
            previous = pY[j];
        }
    }

    previous = 0;
    secondPrevious = 0;

    //Reverse predictor for U and V
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        pU = UComponentReduced.ptr<uchar>(i);
        pUPred = UReducedPredictor.ptr<short>(i);
        pV = VComponentReduced.ptr<uchar>(i);
        pVPred = VReducedPredictor.ptr<short>(i);
        for (int j = 0; j < UComponentReduced.cols; j++)
        {
            pU[j] = pUPred[j] + previous;
            previous = pU[j];

            pV[j] = pVPred[j] + secondPrevious;
            secondPrevious = pV[j];
        }
    }
}