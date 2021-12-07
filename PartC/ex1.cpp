#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

//dá segmentation fault mas é so pra imagem house.ppm

void convertToYUV(const cv::Mat &source, cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent);
void convertTo420(cv::Mat &YComponent, cv::Mat &UComponent, cv::Mat &VComponent, cv::Mat &UComponentReduced, cv::Mat &VComponentReduced);

int main(int argc, char *argv[])
{
    if (argc < 2)
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

    //create new image Matrix
    cv::Mat YComponent = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
    cv::Mat UComponent = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
    cv::Mat VComponent = cv::Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);

    int halfRows = srcImage.rows / 2;
    int halfCols = srcImage.cols / 2;

    //isto aqui dá malloc(), corrupted top size se usar CV_8UC1 para algumas imagens??
    cv::Mat UComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);
    cv::Mat VComponentReduced = cv::Mat::zeros(halfRows, halfCols, CV_8UC1);

    convertToYUV(srcImage, YComponent, UComponent, VComponent);
    convertTo420(YComponent, UComponent, VComponent, UComponentReduced, VComponentReduced);

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

    //copy image pixel by pixel (assuming size of unsigned char for each channel)
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

    //copy image pixel by pixel (assuming size of unsigned char for each channel)
    int nRows = YComponent.rows;
    int nCols = YComponent.cols * channels;

    uchar *pU, *pV, *pUReduced, *pVReduced;
    int countRow = 0;
    int countColums = 0;

    //por cada linha de Y
    for (int i = 0; i < nRows; i++)
    {
        countColums = 0; //dar reset as colunas

        //estes vao andar de 2 em 2 (so conto as linhas pares)
        pU = UComponent.ptr<uchar>(i); //ter o ponteiro pra U
        pV = VComponent.ptr<uchar>(i); //ter o ponteiro pra V

        //ponteiros pra a nova matriz que vao andar de 1 em 1 pra prencher
        pUReduced = UComponentReduced.ptr<uchar>(countRow);
        pVReduced = VComponentReduced.ptr<uchar>(countRow);

        for (int j = 0; j < nCols; j++)
        {
            //aqui as colunas tmb vao andar de 2 em 2 (so conto as colunas pares)
            //atribuo as posiçoes na nova matriz
            pUReduced[countColums] = pU[j];
            pVReduced[countColums] = pV[j];

            //somar + 1 pra saltos de 2 em 2
            j++;
            //somar + 1 pra nova posiçao nas novas matrizes
            countColums++;
        }
        //somar + 1 pra saltos de 2 em 2
        i++;
        //somar + 1 pra nova posiçao nas novas matrizes
        countRow++;
    }
}