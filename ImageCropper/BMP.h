#pragma once
#include <string>
#include <fstream>
#include <array>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>

using namespace std;

typedef unsigned short	USHORT;
typedef unsigned int	UINT;
typedef unsigned long	ULONG;

#define BM					0x4D42			//DIB file is 0x42 0x4D in hexadecimal in header field
#define BI_BITFIELDS		3				//OS22XBITMAPHEADER: Huffman 1D
#define BI_ALPHABITFIELDS	6				//RGBA bit field masks
//#define DEBUG				true			//print log during creation

struct BMP_Header
{
	USHORT	headerField;		//The header field used to identify the BMP
	UINT	fileSize;				//The size of the BMP file in bytes
	USHORT	reservedOne;
	USHORT	reservedTwo;
	UINT	offset;				//The offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found.
};


struct DIB_Header
{
	UINT	headerSize;				//	the size of this header, in bytes (40)
	UINT	width;					//	the bitmap width in pixels (signed integer)
	UINT	height;					//	the bitmap width in pixels (signed integer)
	USHORT	colorPlane;				//	the number of color planes (must be 1)
	USHORT	depth;					//	the number of bits per pixel, which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.
	UINT	compressionMethod;		//	the compression method being used. 
	UINT	sizeOfRawBitmap;		//	the image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.
	UINT	horizontalResolution;	//	the horizontal resolution of the image. (pixel per metre, signed integer)
	UINT	verticalResolution;		//	the vertical resolution of the image. (pixel per metre, signed integer)
	UINT	numberOfColors;			//	the number of colors in the color palette, or 0 to default to 2^n
	UINT	importantColor;			//	the number of important colors used, or 0 when every color is important; generally ignored
};


struct Color_Definition
{
	char b;
	char g;
	char r;
	char alpha;
};


struct Position
{
	UINT x;
	UINT y;
};


class BMP
{
public:
	BMP(const string& file);
	~BMP();
	BMP_Header GetBMP_Header();
	DIB_Header GetDIB_Header();
	vector<char> GetColorTable();
	vector<char> GetImageData();
	vector<vector<int> > GetPixelArray();
protected:
	BMP_Header bmp_header;
	DIB_Header dib_header;
	vector<char>  colorTable;
	vector<char>  imageData;
	vector<vector<int> > pixelsArray;
private:
	static constexpr size_t BMP_HEADER_SIZE = 14;
	static constexpr size_t DIB_HEADER_SIZE = 40;
	static constexpr size_t BITS_PER_PIXEL = 8;
	static constexpr size_t RGB32 = 4;
	void loadBMP_Header(ifstream& inputfile);
	void loadDIB_Header(ifstream& inputfile);
	void loadColorTable(ifstream& inputfile);
	void loadImageData(ifstream& inputfile);
	void convertPixels(vector<char>  imageData);
};