#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <iomanip>
#include <stdlib.h>


using namespace cv;

// calculating the median for four pixel
double median(Vec4b list, std::string o) {
    int length = list.rows;
    double median;

    sort(list, list, SORT_EVERY_COLUMN);
    
    if (length % 2 == 0 && o == "0") {
        median = list[(length / 2)];
    }
    else if (length % 2 == 0 && o == "1") {
        median = list[(length / 2) + 1];
    }
    else if (length % 2 == 0 && o == "2") {
        median = (list[(length / 2)] + list[((length / 2) + 1)]) / 2;
    }
    else if (length % 2 != 0) {
        median = list[(int) length / 2];
    }

    return median;
}

// calculating the median for nine pixel
double median2(int list[9]) {
    int length = 9;
    double median;

    for(int i = 0; i < length - 1; i++) {
        int minNum = i;

        for (int j = i + 1; j < length; j++) {
            if (list[j] < list[minNum]) {
                minNum = j;
            }
        }

        int temp = list[i];
        list[i] = list[minNum];
        list[minNum] = temp;
    }

    if (length % 2 == 0) {
        median = list[(length / 2)];
    }
    else if(length % 2 != 0) {
        median = list[(int)length / 2];
    }

    return median;
}

// calling the median function for each pixel of the new image
void scaling(Mat imgIn, Mat imgOut, std::string o) {

    float multiRows = imgIn.rows / imgOut.rows;
    float multiCols = imgIn.cols / imgOut.cols;

    if (o == "0" || o == "1" || o == "2") {
        for (int r = 0; r < imgOut.rows; r++) {
            for (int c = 0; c < imgOut.cols; c++) {
                Vec3b p;

                for (int i = 0; i < 3; i++) {
                    Vec4b color;

                    color[0] = imgIn.at<Vec3b>(r * multiRows, c * multiCols)[i];
                    color[1] = imgIn.at<Vec3b>(r * multiRows, c * multiCols + 1)[i];
                    color[2] = imgIn.at<Vec3b>(r * multiRows + 1, c * multiCols)[i];
                    color[3] = imgIn.at<Vec3b>(r * multiRows + 1, c * multiCols + 1)[i];

                    p[i] = median(color, o);
                }
                imgOut.at<Vec3b>(r, c) = p;
            }
        }
    }
    else if (o == "3") {
        for (int r = 0; r < imgOut.rows; r++) {
            for (int c = 0; c < imgOut.cols; c++) {
                Vec3b p;

                for (int i = 0; i < 3; i++) {
                    int color[9];

                    int col = c * multiCols;
                    int row = r * multiRows;
                    if (imgIn.cols <= (col + 2)) {
                        col = c * multiCols - 1;
                    }
                    if (imgIn.rows <= (row + 2)) {
                        row = r * multiRows - 1;
                    }

                    color[0] = imgIn.at<Vec3b>(row, col)[i];
                    color[1] = imgIn.at<Vec3b>(row, col + 1)[i];
                    color[2] = imgIn.at<Vec3b>(row, col + 2)[i];
                    color[3] = imgIn.at<Vec3b>(row + 1, col)[i];
                    color[4] = imgIn.at<Vec3b>(row + 1, col + 1)[i];
                    color[5] = imgIn.at<Vec3b>(row + 1, col + 2)[i];
                    color[6] = imgIn.at<Vec3b>(row + 2, col)[i];
                    color[7] = imgIn.at<Vec3b>(row + 2, col + 1)[i];
                    color[8] = imgIn.at<Vec3b>(row + 2, col + 2)[i];

                    p[i] = median2(color);
                }
                imgOut.at<Vec3b>(r, c) = p;
            }
        }
    }
}

int main(int ac, char** av)
{
    std::cout << "Image Path: " << std::endl;
    std::string imagePath;
    std::getline(std::cin, imagePath);

    std::cout << "Safe Path: " << std::endl;
    std::string safePath;
    std::getline(std::cin, safePath);

    std::cout << "Operator: 0 - CMF with 4 Pixel using the second Pixel. 1 - CMF with 4 Pixel using the third Pixel. 2 - CMF with 4 Pixel using the average. 3 - CMF with 9 Pixel" << std::endl;
    std::string o;
    std::getline(std::cin, o);

    Mat imgIn = imread(imagePath, IMREAD_COLOR);
    Mat imgOut;
    Mat sizeImg = imread(safePath + "bicubic.png", IMREAD_COLOR); // getting the size of the compared images
    
    if (imgIn.empty())
    {
        std::cout << "Could not read the image: " << imagePath << std::endl;
        return 1;
    }

    //int rowsSize = imgIn.rows * 0.25; // testing different scalings
    //int colsSize = imgIn.cols * 0.25; // testing different scalings
    int rowsSize = sizeImg.rows;
    int colsSize = sizeImg.cols;
    
    if (imgIn.rows % sizeImg.rows == 0) {
        rowsSize = sizeImg.rows;
    }
    else {
        int rowsCount = (int) imgIn.rows / sizeImg.rows;
        rowsSize = imgIn.rows / rowsCount;
    }

    if (imgIn.cols % sizeImg.cols == 0) {
        colsSize = sizeImg.cols;
    }
    else {
        int colsCount = (int) imgIn.cols / sizeImg.cols;
        colsSize = imgIn.cols / colsCount;
    }

    resize(imgIn, imgOut, Size(colsSize, rowsSize));

    scaling(imgIn, imgOut, o);

    imshow("Display window", imgIn);
    imshow("Resized", imgOut);
    
    Mat upscaled;
    resize(imgOut, upscaled, Size((imgIn.cols), (imgIn.rows)), INTER_NEAREST);
    
    std::string endPath;
    if (o == "0") {
        endPath = "cmf4a";
    }
    else if (o == "1") {
        endPath = "cmf4b";
    }
    else if (o == "2") {
        endPath = "cmf4c";
    }
    else if (o == "3") {
        endPath = "cmf9";
    }

    int k = waitKey(0);
    if (k == 's')
    {
        std::string safe = safePath + endPath + ".png";
        imwrite(safe, imgOut);

        std::string safeUp = safePath + "/Upscaled/" + endPath + "_upscaled.png";
        imwrite(safeUp, upscaled);
    }
    return 0;
}