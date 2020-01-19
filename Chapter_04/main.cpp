//
// Created by expoli on 2020/1/14.
//

#include <iostream>
#include <string>

using namespace std;

// Opencv includes
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"

using namespace cv;

// OpenCV command line parser functions
// Keys accepted by command line parser
const char *keys = {
        "{ help h usage ? | | print this message}"
        "{@image | | Image to process}"
};

void showHistoCallback(int state, void *userData);

void equalizeCallback(int state, void *userData);

void lomoCallback(int state, void *userData);

void cartoonCallback(int state, void *userData);

Mat img;

int main(int argc, const char **argv) {
    CommandLineParser parser(argc, argv, keys);
    parser.about("Chapter 4. PhotoTool V1.0.0");
    // If requires help show
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    String imgFile = parser.get<String>(0);

    // Check if params are correctly parsed in his variables
    if (!parser.check()) {
        parser.printErrors();
        return 0;
    }

    // Load image to process
    if (imgFile.empty())
        img = imread("../lena.jpg");
    else
        img = imread(imgFile);

    // Create window
    namedWindow("Input");

    // Create UI buttons
    createButton("Show histogram", showHistoCallback, NULL, QT_CHECKBOX, false);
    createButton("Equalize histogram", equalizeCallback, NULL, QT_CHECKBOX, false);
    createButton("Lomography effect", lomoCallback, NULL, QT_CHECKBOX, false);
    createButton("Cartoonize effect", cartoonCallback, NULL, QT_CHECKBOX, false);

    // Show image
    imshow("Input", img);

    waitKey(0);
    return 0;
}

void showHistoCallback(int state, void *userData) {
    if (state) {

        // Separate image in BRG
        vector<Mat> bgr;    // 向量类型储存每个通道
        split(img, bgr);    // split 函数将图形分为三个通道

        // Create the histogram for 256 bins
        // The number of possibles values [0..255]
        int numbins = 256;  // 定义直方图的区间

        // Set the rangers for B,G,R last is not included
        float range[] = {0, 256};   // 定义变量范围
        const float *histRange = {range};

        Mat b_hist, g_hist, r_hist; // 定义三个储存矩阵、分别储存每个直方图
        // calcHist 计算直方图
        // 参数
        // 输入图像 用于计算直方图的输入图像数 用于计算直方图的数字通道尺寸 可选的掩码矩阵 用于储存计算得到的直方图的变量 直方图维度 要计算的区间数 输入变量的范围
        calcHist(&bgr[0], 1, 0, Mat(), b_hist, 1, &numbins, &histRange);
        calcHist(&bgr[1], 1, 0, Mat(), g_hist, 1, &numbins, &histRange);
        calcHist(&bgr[2], 1, 0, Mat(), r_hist, 1, &numbins, &histRange);

        // Draw the histogram
        // Wo go to draw lines for each channel
        int width = 512;    // 创建 512*300 像素大小的彩色图像
        int height = 300;

        // Create image with gray base
        Mat histImage(height, width, CV_8UC3, Scalar(20, 20, 20));

        // Normalize the histograms to height of image
        // 在最小值和最大值之间标准化直方图，最大值与输出直方图图像的高度相同
        normalize(b_hist, b_hist, 0, height, NORM_MINMAX);
        normalize(g_hist, g_hist, 0, height, NORM_MINMAX);
        normalize(r_hist, r_hist, 0, height, NORM_MINMAX);

        int binStep = cvRound((float) width / (float) numbins);
        for (int i = 0; i < numbins; i++) {
            line(histImage, Point(binStep * (i - 1), height - cvRound(b_hist.at<float>(i - 1))),
                 Point(binStep * (i), height - cvRound(b_hist.at<float>(i))),
                 Scalar(255, 0, 0));
            line(histImage, Point(binStep * (i - 1), height - cvRound(g_hist.at<float>(i - 1))),
                 Point(binStep * (i), height - cvRound(g_hist.at<float>(i))),
                 Scalar(0, 255, 0));
            line(histImage, Point(binStep * (i - 1), height - cvRound(r_hist.at<float>(i - 1))),
                 Point(binStep * (i), height - cvRound(r_hist.at<float>(i))),
                 Scalar(0, 0, 255));
        }

        imshow("Histogram", histImage);
    } else {
        destroyWindow("Histogram");
    }
}

void equalizeCallback(int state, void *userData) {
    if (state) {
        Mat result;
        // Convert BGR image to YCbCr
        Mat ycrcb;
        cvtColor(img, ycrcb, COLOR_BGR2YCrCb);

        // split image into channels
        vector<Mat> channels;
        split(ycrcb, channels);

        // Equalize the Y channel only
        equalizeHist(channels[0], channels[0]);

        // Merge the result channels
        merge(channels, ycrcb);

        cvtColor(ycrcb, result, COLOR_YCrCb2BGR);

        // show image
        imshow("Equalized", result);
    } else {
        destroyWindow("Equalized");
    }
}

void lomoCallback(int state, void *userData) {
    if (state) {
        Mat result;
        const double exponential_e = exp(1.0);
        // Create Look-up table for color curve effect
        Mat lut(1, 256, CV_8UC1);
        for (int i = 0; i < 256; i++) {
            float x = (float) i / 256.0;
            lut.at<uchar>(i) = cvRound(256 * (1 / (1 + pow(exponential_e, -((x - 0.5) / 0.1)))));
        }
        // Split the image channels and apply curve transform only to red channel
        vector<Mat> bgr;
        split(img, bgr);
        // 通过使用查找表将一个曲线应用于红色通道来实现颜色操作效果
        LUT(bgr[2], lut, bgr[2]);
        // merge result
        merge(bgr, result);
        // Create image for halo dark
        // 通过对图像应有暗晕来实现复古效果
        Mat halo(img.rows, img.cols, CV_32FC3, Scalar(0.3, 0.3, 0.3));
        // Create circle
        circle(halo, Point(img.cols / 2, img.rows / 2), img.cols / 3, Scalar(1, 1, 1), -1);
        // 使用 blur 滤镜函数对圆光晕应用大模糊，以获得平滑效果
        blur(halo, halo, Size(img.cols / 3, img.cols / 3));
        // Convert the result to float to allow multiply by 1 factor
        Mat resultf;
        // 将输入图像从8位图像转化为32位浮点数，需要把0-1范围内的模糊图像与具有整数值的输入图像相乘。
        result.convertTo(resultf, CV_32FC3);
        // Multiply our result with halo
        multiply(resultf, halo, resultf);
        // convert to 8 bits
        resultf.convertTo(result, CV_8UC3);

        // show result
        imshow("Lomography", result);
    } else {
        destroyWindow("Lomography");
    }
}

void cartoonCallback(int state, void *userData) {
    if (state) {
        // EDGES
        // Apply median filter to remove possible noise
        Mat imgMedian;
        medianBlur(img, imgMedian, 7);

        // Detect edges with canny
        Mat imgCanny;
        Canny(imgMedian, imgCanny, 50, 150);
        // Dilate the edges
        Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
        dilate(imgCanny, imgCanny, kernel);

        // Scale edges values to 1 and invert values
        imgCanny = imgCanny / 255;
        imgCanny = 1 - imgCanny;
        // Use float values to allow multiply between 0 and 1
        Mat imgCannyf;
        imgCanny.convertTo(imgCannyf, CV_32FC3);

        // Blur the edgest to do smooth effect
        blur(imgCannyf, imgCannyf, Size(5, 5));

        // Color
        // Apply bilateral filter to homogenizes color
        Mat imgBF;
        bilateralFilter(img, imgBF, 9, 150.0, 150.0);

        // truncate colors
        Mat result = imgBF / 25;
        result = result * 25;

        // MERGES COLOR + EDGES
        // Create a 3 channels for edges
        Mat imgCanny3c;
        Mat cannyChannels[] = {imgCannyf, imgCannyf, imgCannyf};
        merge(cannyChannels, 3, imgCanny3c);
        // Convert color result to float
        Mat resultf;
        result.convertTo(resultf, CV_32FC3);

        // Multiply color and edges matrices
        multiply(resultf, imgCanny3c, resultf);

        // Convert to 8 bits color
        resultf.convertTo(result, CV_8UC3);

        // show image
        imshow("Cartoon", result);
    } else {
        destroyWindow("Cartoon");
    }
}