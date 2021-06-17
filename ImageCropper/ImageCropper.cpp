// ImageCropper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
using namespace std;
#include "BMPEditer.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Please check the input format and try again" << endl;
        cout << "Name: ImageCropper.exe" << endl;
        cout << "Description: A commandline tool that crops an image to a smallest bounding rectangle." << endl;
        cout << "Format: ImageCroper.exe xxxx.bmp" << endl;
        return 0;
    }
        
    if (argc == 2)
    {
        string pathname = argv[argc - 1];
        BMPEditer inputFile = BMPEditer(pathname);
        inputFile.TrimBoundingRect("fozzie-out.bmp");
        cout << pathname << " is edited, and the output file, fozzie-out.bmp is saved at same folder." << endl;
    }

    return 0;
}



