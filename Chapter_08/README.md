# Chapter 08. Video Surveillance, Background Modeling, and Morphological Operations 

## Requirements

- OpenCV 4.0
- WebCam

## Steps to build

To compile on Linux, mac or Windows using Mingw

```
mkdir build
cd build
cmake ..
make
```

## Executables

The following applications are generated.

```
./backgroundSubtraction
./dilation  ../resources/test.png 5
./erosion  ../resources/test.png 5
./frameDifferencing  
./morphologicalOperations ../resources/test.png 5

```
