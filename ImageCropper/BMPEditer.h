#pragma once
#include "BMP.h"


struct File_Structure
{
	BMP_Header bmp_header;
	DIB_Header dib_header;
	vector<char> colorTable;
	vector<char> imageData;
};

struct Position_Area
{
	UINT startX;
	UINT endX;
	UINT startY;
	UINT endY;
};

struct Bound_Info
{
	Position bottomPos;
	Position rightPos;
	Position topPos;
	Position leftPos;
};




class BMPEditer : public BMP
{
public:
	BMPEditer(const string& file);
	~BMPEditer();
	
	void BoundingRect(const char* fileName);
	void TrimBoundingRect(const char* fileName);
private:
	char originalPixel;
	Bound_Info boundInfo;
	void writeHeaders(ofstream& file);
	void findTheUniquePosBT(vector<vector<int> >& originalPixelArray, Position_Area area, Position& bottomPos, Position& topPos);
	void findTheUniquePosLR(vector<vector<int> >& originalPixelArray, Position_Area area, Position& positionL, Position& positionR);
	void editeThePixelArray(vector<vector<int> >& originalPixelArray, Position_Area area);
	void updateImageData(vector<vector<int> >& newPixelArray, vector<char>& newImageDate, Position_Area area, int padRowSize);
	void CreateNewBMP(File_Structure file_structure, const char* fileName);
	int calculatePadRowSize(UINT startX, UINT endX);
	void editFileStructure(File_Structure& fileStructure, Position_Area area, int padRowSize);
};