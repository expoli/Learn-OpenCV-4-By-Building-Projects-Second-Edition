// FACE DETECTION

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>

#define CV_HAAR_SCALE_IMAGE 2

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
    // 检测面部部分的一个分类器 xml
//    string faceCascadeName = "../resources/haarcascade_frontalface_alt.xml";
    string faceCascadeName = argv[1];
    CascadeClassifier faceCascade;

    if (!faceCascade.load(faceCascadeName)) {
        cerr << "Error loading cascade file. Exiting!" << endl;
        return -1;
    }
    // 面具图像文件
    Mat faceMask = imread(argv[2]);
    if (!faceMask.data) {
        cerr << "Error loading mask image. Exiting!" << endl;
    }

    // Current frame
    Mat frame, frameGray;
    Mat frameROI, faceMaskSmall;
    Mat grayMaskSmall, grayMaskSmallThresh, grayMaskSmallThreshInv;
    Mat maskedFace, maskedFrame;

    // Create the capture object
    // 0 -> input arg that specifies it should take the input from the webcam
    VideoCapture cap(0);

    // If you cannot open the webcam, stop the execution!
    if (!cap.isOpened())
        return -1;

    //create GUI windows
    namedWindow("Frame");

    // Scaling factor to resize the input frames from the webcam
    float scalingFactor = 0.75;

    vector<Rect> faces;

    // Iterate until the user presses the Esc key
    while (true) {
        // Capture the current frame
        cap >> frame;

        // Resize the frame
        resize(frame, frame, Size(), scalingFactor, scalingFactor, INTER_AREA);

        // Convert to grayscale
        // 转换为灰度图、面部检测适用于灰度图像
        cvtColor(frame, frameGray, COLOR_BGR2GRAY);

        // Equalize the histogram
        // 均衡直方图、补偿照明或饱和度等问题、保证图像具有健康的像素值范围
        equalizeHist(frameGray, frameGray);

        // Detect faces
        faceCascade.detectMultiScale(frameGray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

        // Draw green rectangle around the face
        for (auto &face: faces) {

            // Custom parameters to make the mask fit your face. You may have to play around with them to make sure it works.
            int x = face.x - int(0.2 * face.width);
            int y = face.y - int(0.0 * face.height);
            int w = int(1.2 * face.width);
            int h = int(1.2 * face.height);
            // 检测数据是否超出图像范围、如果超出范围、frameROI = frame(Rect(x, y, w, h)); 调用时会引起程序报错
            if (x > 0 && y > 0 && x + w < frame.cols && y + h < frame.rows) {
                // Extract region of interest (ROI) covering your face
                frameROI = frame(Rect(x, y, w, h));

                // Resize the face mask image based on the dimensions of the above ROI
                resize(faceMask, faceMaskSmall, Size(w, h));

                // Convert the above image to grayscale
                cvtColor(faceMaskSmall, grayMaskSmall, COLOR_BGR2GRAY);

                // Threshold the above image to isolate the pixels associated only with the face mask
                threshold(grayMaskSmall, grayMaskSmallThresh, 245, 255, THRESH_BINARY_INV);

                // Create mask by inverting the above image (because we don't want the background to affect the overlay)
                bitwise_not(grayMaskSmallThresh, grayMaskSmallThreshInv);

                // Use bitwise "AND" operator to extract precise boundary of face mask
                bitwise_and(faceMaskSmall, faceMaskSmall, maskedFace, grayMaskSmallThresh);

                // Use bitwise "AND" operator to overlay face mask
                bitwise_and(frameROI, frameROI, maskedFrame, grayMaskSmallThreshInv);

                // Add the above masked images and place it in the original frame ROI to create the final image

                add(maskedFace, maskedFrame, frame(Rect(x, y, w, h)));
            } else {
                // 打印超出范围的数据
                cout << x << " " << y << " " << w << " " << h << endl;
            }
        }

        // Show the current frame
        imshow("Frame", frame);

        // Get the keyboard input and check if it's 'Esc'
        // 27 -> ASCII value of 'Esc' key
        auto ch = waitKey(30);
        if (ch == 27) {
            break;
        }
    }

    // Release the video capture object
    cap.release();

    // Close all windows
    destroyAllWindows();

    return 0;
}