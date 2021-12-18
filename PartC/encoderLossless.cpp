#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "../PartA_1/classes/Golomb.h"

using namespace std;
using namespace cv;

void convertToYUV(const cv::Mat &source, cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent);
void convertTo420(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);
void predictor1(cv::Mat &YComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced, cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);
uint32_t getOptimalM(cv::Mat &YPredictor, cv::Mat &UReducedPredictor, cv::Mat &VReducedPredictor);
void writeMatlabVectorFiles(map<int, double> &mapY, map<int, double> &mapU, map<int, double> &mapV);

//./encoder image textFile
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

    //Encode Header with fixed m = 200
    Golomb encoder((string)argv[2], BitStream::bs_mode::write, 200);

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

    imwrite("Y.jpeg", YComponent);
    imwrite("U.jpeg", UComponent);
    imwrite("V.jpeg", VComponent);

    imwrite("UReduced.jpeg", UComponentReduced);
    imwrite("VReduced.jpeg", VComponentReduced);

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

    double entropyY, entropyU, entropyV;
    map<int, double> probMapY, probMapU, probMapV;
    double totalBlue = 0, totalGreen = 0, totalRed = 0;

    for (int i = -255; i < 256; i++)
    {
        probMapY[i] = mapY[i] / totalY;
        probMapU[i] = mapU[i] / totalU;
        probMapV[i] = mapV[i] / totalV;

        cout << "prob map Y: " << probMapY[i] << endl;

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

        totalBlue += probMapY[i];
        totalGreen += probMapU[i];
        totalRed += probMapV[i];
    }

    std::cout << "Y prob total"
              << ": " << totalBlue << '\n';

    std::cout << "U prob total"
              << ": " << totalGreen << '\n';

    std::cout << "V prob total"
              << ": " << totalRed << '\n';

    std::cout << "Y entropy"
              << ": " << -entropyY << '\n';

    std::cout << "U entropy"
              << ": " << -entropyU << '\n';

    std::cout << "V entropy"
              << ": " << -entropyV << '\n';

    writeMatlabVectorFiles(mapY, mapU, mapV);

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
    std::ofstream YFrequenceFile("../matlab/YFrequence.txt");
    std::ofstream UFrequenceFile("../matlab/UFrequence.txt");
    std::ofstream VFrequenceFile("../matlab/VFrequence.txt");

    for (int i = -255; i < 256; i++)
    {
        xAxisFile << i << std::endl;
        YFrequenceFile << mapY[i] << std::endl;
        UFrequenceFile << mapU[i] << std::endl;
        VFrequenceFile << mapV[i] << std::endl;
    }

    YFrequenceFile.close();
    UFrequenceFile.close();
    VFrequenceFile.close();
}