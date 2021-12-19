#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

void reversePredictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals);

//./decoder textFile
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: ./decoderLossy TextFile" << endl;
        return -1;
    }

    int32_t m, rows, cols, reduceY, reduceU, reduceV;

    //Decode header with fixed m = 400
    Golomb decoder((string)argv[1], BitStream::bs_mode::read, 400);

    m = decoder.decodeNumber();
    rows = decoder.decodeNumber();
    cols = decoder.decodeNumber();
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

    //Acho q esta a dar overflow nos shifts ns, apartir de 8 de reduçao é irreconhecivel

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

    reversePredictor1(YComponent, UComponentReduced, VComponentReduced, YResiduals, UReducedResiduals, VReducedResiduals);

    imwrite("YDecoded.jpeg", YComponent);

    imwrite("UReducedDecoded.jpeg", UComponentReduced);
    imwrite("VReducedDecoded.jpeg", VComponentReduced);

    return 0;
}

void reversePredictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YResiduals, cv::Mat &UReducedResiduals, cv::Mat &VReducedResiduals)
{

    uchar *pY, *pU, *pV;
    short *pYRes, *pURes, *pVRes;

    //Reverse predictor for Y
    for (int i = 0; i < YComponent.rows; i++)
    {
        pY = YComponent.ptr<uchar>(i);
        pYRes = YResiduals.ptr<short>(i);
        pY[0] = pYRes[0];
        for (int j = 1; j < YComponent.cols; j++)
        {
            pY[j] = pYRes[j] + pY[j - 1];
        }
    }

    //Reverse predictor for U and V
    for (int i = 0; i < UComponentReduced.rows; i++)
    {
        pU = UComponentReduced.ptr<uchar>(i);
        pURes = UReducedResiduals.ptr<short>(i);
        pV = VComponentReduced.ptr<uchar>(i);
        pVRes = VReducedResiduals.ptr<short>(i);
        pU[0] = pURes[0];
        pV[0] = pVRes[0];
        for (int j = 1; j < UComponentReduced.cols; j++)
        {
            pU[j] = pURes[j] + pU[j - 1];

            pV[j] = pVRes[j] + pV[j - 1];
        }
    }
}