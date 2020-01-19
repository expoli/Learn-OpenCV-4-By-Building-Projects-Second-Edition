//
// Created by expoli on 2020/1/13.
//

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/freeglut.h"

using namespace cv;

Mat frame;
GLfloat angle = 0.0;
GLuint texture;
VideoCapture camera;

int loadTexture() {
    if (frame.data == NULL)
        return -1;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data);

    return 0;
}

void on_opengl(void *param) {
    glLoadIdentity();
    // Load frame Texture
    glBindTexture(GL_TEXTURE_2D, texture);
    // Rotate plane before draw
    glRotatef(angle, 1.0f, 1.0f, 1.0f);
    // Create the plane and set the texture coordinates
    glBegin(GL_QUADS);
    // first point and coordinate texture
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-1.0, -1.0);
    // second point and coordinate texture
    glTexCoord2d(1.0, 0.0);
    glVertex2d(+1.0, -1.0);
    // third point and coordinate texture
    glTexCoord2d(1.0, 1.0);
    glVertex2d(+1.0, +1.0);
    // last point coordinate texture
    glTexCoord2d(0.0, 1.0);
    glVertex2d(-1.0, +1.0);

    glEnd();
}

int main(int argc, const char **argv) {
    // open webcam
    camera.open(0);
    if (!camera.isOpened())
        return -1;

    // Create new window
    namedWindow("OpenGL Camera", WINDOW_OPENGL);
    // Enable texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    setOpenGlDrawCallback("OpenGL Camera", on_opengl);
    while (waitKey(30) != 'q') {
        camera >> frame;
        // Create first texture
        loadTexture();
        updateWindow("OpenGL Camera");
        angle = angle + 4;
    }
    // Destroy the window
    destroyWindow("OpenGL Camera");

    return 0;
}