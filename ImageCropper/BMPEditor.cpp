#include "BMPEditor.h"


BMPEditor::BMPEditor(const string& file) : BMP(file)
{

}


void BMPEditor::BoundingRect(const char* fileName)
{
	vector<vector<int> > originalPixelArray = this->GetPixelArray();

	UINT pixelWidth = originalPixelArray.size();
	UINT pixelHeight = originalPixelArray[0].size();

	if (pixelWidth == 0 || pixelHeight == 0)
		throw "no pixel information existed!!!";

	// Not sure how to chose the original Pixel, should be tweak in the future
	if ((originalPixelArray[0][0] == originalPixelArray[pixelWidth - 1][0])
		&& (originalPixelArray[pixelWidth - 1][0]) == (originalPixelArray[0][pixelHeight - 1])
		&& (originalPixelArray[0][pixelHeight - 1] == originalPixelArray[pixelWidth - 1][pixelHeight - 1]))
	{
		originalPixel = originalPixelArray[0][0];
	}
	else
		throw "this image may be trimmed already";

	Position_Area area;
	// Check Bottom Area and Top Area
	area.startX = 0;
	area.endX = pixelWidth;
	area.startY = 0;
	area.endY = pixelHeight;
	findTheUniquePosBT(originalPixelArray, area, boundInfo.bottomPos, boundInfo.topPos);

	// Check Right Area and Left Area
	area.startX = 0;
	area.endX = pixelWidth;
	//Y area should be remaining of the top-bottom scan
	area.startY = boundInfo.bottomPos.y;
	area.endY = boundInfo.topPos.y + 1;
	findTheUniquePosLR(originalPixelArray, area, boundInfo.leftPos, boundInfo.rightPos);

	////Create new BMP with bounding rect
	//File_Structure newFileStructure;
	//newFileStructure.bmp_header = this->GetBMP_Header();
	//newFileStructure.dib_header = this->GetDIB_Header();
	//newFileStructure.colorTable = this->GetColorTable();
	//vector<char> newImageData;
	//Position_Area newArea;
	//newArea.startX = 0;
	//newArea.endX = pixelWidth;
	//newArea.startY = 0;
	//newArea.endY = pixelHeight;
	//updateImageData(originalPixelArray, newImageData, newArea, 0);
	//newFileStructure.imageData = newImageData;
	//this->CreateNewBMP(newFileStructure, "fozzie-boundingrect.bmp");

}

void BMPEditor::TrimBoundingRect(const char* fileName) 
{
	BoundingRect(fileName);
	Position_Area newArea;
	newArea.startX = (boundInfo.bottomPos.x < boundInfo.leftPos.x) ? boundInfo.bottomPos.x : boundInfo.leftPos.x;
	newArea.startY = (boundInfo.bottomPos.y < boundInfo.leftPos.y) ? boundInfo.bottomPos.y : boundInfo.leftPos.y;
	newArea.endX = ((boundInfo.topPos.x > boundInfo.rightPos.x) ? boundInfo.topPos.x : boundInfo.rightPos.x) + 1; // if conditional statement need 1 more for end
	newArea.endY = ((boundInfo.topPos.y > boundInfo.rightPos.y) ? boundInfo.topPos.y : boundInfo.rightPos.y) + 1; // if conditional statement need 1 more for end

	//cout << "(" << newArea.startX << ", " << newArea.startY << ")" << "(" << newArea.endX << ", " << newArea.endY << ")" << endl;
	vector<vector<int> > newPixelArray = vector<vector<int> >(newArea.endX - newArea.startX, vector<int>(newArea.endY - newArea.startY));
	int padRowSize = calculatePadRowSize(newArea.startX, newArea.endX);

	//Create new BMP without bounding rect
	File_Structure newFileStructure;
	this->editFileStructure(newFileStructure, newArea, padRowSize);
	this->CreateNewBMP(newFileStructure, fileName);
}


void BMPEditor::editFileStructure(File_Structure& fileStructure, Position_Area area, int padRowSize)
{
	vector<vector<int> > originalPixelArray = this->GetPixelArray();
	//edit the image data
	vector<char> newImageData;
	updateImageData(originalPixelArray, newImageData, area, padRowSize);
	fileStructure.imageData = newImageData;

	//the color table should same as source
	fileStructure.colorTable = this->GetColorTable();

	//edit the DIB header infomation, only the width and height related information
	DIB_Header newDIB_Header = this->GetDIB_Header();
	newDIB_Header.width = area.endX - area.startX;
	newDIB_Header.height = area.endY - area.startY;
	newDIB_Header.sizeOfRawBitmap = newImageData.size();
	fileStructure.dib_header = newDIB_Header;
	//edit the bmp hearder information
	BMP_Header newBMP_Header = this->GetBMP_Header();
	newBMP_Header.fileSize = newBMP_Header.offset + newDIB_Header.sizeOfRawBitmap;
	fileStructure.bmp_header = newBMP_Header;
}



int BMPEditor::calculatePadRowSize(UINT startX, UINT endX)
{
	return (endX - startX) % 4;
}

void BMPEditor::CreateNewBMP(File_Structure file_structure, const char * fileName)
{
	FILE* outputFile;
	fopen_s(&outputFile, fileName, "wb");		//Truncate to zero length or create file for writing.
	if (!outputFile) {
		throw "File opening failed";
	}
	fwrite((const char*)&file_structure.bmp_header.headerField, 1, 2, outputFile);
	fwrite((const char*)&file_structure.bmp_header.fileSize, 1, 4, outputFile);
	fwrite((const char*)&file_structure.bmp_header.reservedOne, 1, 2, outputFile);
	fwrite((const char*)&file_structure.bmp_header.reservedTwo, 1, 2, outputFile);
	fwrite((const char*)&file_structure.bmp_header.offset, 1, 4, outputFile);
	fwrite((const char*)&file_structure.dib_header, 1, sizeof(file_structure.dib_header), outputFile);
	fwrite(&file_structure.colorTable[0], 1, file_structure.colorTable.size(), outputFile);
	fwrite(&file_structure.imageData[0], 1, file_structure.imageData.size(), outputFile);

	fclose(outputFile);
}



void BMPEditor::updateImageData(vector<vector<int> >& newPixelArray, vector<char>& newImageDate, Position_Area area, int padRowSize)
{
	for (UINT positionY = area.startY; positionY < area.endY; positionY++)
	 {    
		for (UINT positionX = area.startX; positionX < area.endX; positionX++)
		{
			newImageDate.push_back(newPixelArray[positionX][positionY]);
		}
		for (int index = 0; index < padRowSize; index++)
		{
			newImageDate.push_back(0); //pad row 
		}
	}
}

void BMPEditor::editeThePixelArray(vector<vector<int> >& originalPixelArray, Position_Area area)
{
	for (UINT positionX = area.startX; positionX < area.endX; positionX++)
	{
		for (UINT positionY = area.startY; positionY < area.endY; positionY++)
		{
			originalPixelArray[positionX][positionY] = 255;
		}
	}
}

void BMPEditor::findTheUniquePosBT(vector<vector<int> >& originalPixelArray, Position_Area area, Position& bottomPos, Position& topPos)
{
	bool bottomFlag = false;
	bool topFlag = false;
	Position_Area updateArea;
	for (UINT positionY = area.startY; positionY < area.endY; positionY++)
	{
		for (UINT positionX = area.startX; positionX < area.endX; positionX++)
		{
			if (!bottomFlag)
			{
				if (originalPixelArray[positionX][positionY] != originalPixel)
				{
					bottomFlag = true;
					bottomPos.x = positionX;
					bottomPos.y = positionY;
				}
				if (positionX == area.endX - 1 && !bottomFlag)
				{
					updateArea.startX = area.startX;
					updateArea.endX = area.endX;
					updateArea.startY = positionY;
					updateArea.endY = positionY + 1;
					editeThePixelArray(originalPixelArray, updateArea);
				}
			}
			if (!topFlag)
			{
				if (originalPixelArray[area.endX - positionX - 1][area.endY - positionY - 1] != originalPixel)
				{
					topFlag = true;
					topPos.x = area.endX - positionX - 1;
					topPos.y = area.endY - positionY - 1;
				}

				if (positionX == area.endX - 1 && !topFlag) //true: positionX = area.endX - 1
				{
					updateArea.startX = area.startX;
					updateArea.endX = area.endX;
					updateArea.startY = area.endY - positionY - 1;
					updateArea.endY = area.endY - positionY;
					editeThePixelArray(originalPixelArray, updateArea);
				}
			}

			if (bottomFlag && topFlag)
			{
				#ifdef DEBUG
					cout << bottomPos.x << "," << bottomPos.y << endl;
					cout << topPos.x << "," << topPos.y << endl;
				#endif
				return;
			}
				
		}
	}
	throw "Doesn't find a unique pixel";
}

void BMPEditor::findTheUniquePosLR(vector<vector<int> >& originalPixelArray, Position_Area area, Position& leftPos, Position& rightPos)
{
	bool leftFlag = false;
	bool rightFlag = false;
	Position_Area updateArea;

	for (UINT positionX = area.startX; positionX < area.endX; positionX++)
	{	
		for (UINT positionY = area.startY; positionY < area.endY; positionY++)
		{
			if (!leftFlag)
			{
				if (originalPixelArray[positionX][positionY] != originalPixel)
				{
					leftFlag = true;
					leftPos.x = positionX;
					leftPos.y = positionY;
				}
				if (positionY == area.endY - 1 && !leftFlag) //true: positionX = area.endX - 1
				{
					updateArea.startX = positionX;
					updateArea.endX = positionX + 1;
					updateArea.startY = area.startY;
					updateArea.endY = area.endY;
					editeThePixelArray(originalPixelArray, updateArea);
				}
			}
			if (!rightFlag)
			{
				if (originalPixelArray[area.endX - positionX - 1][area.startY + area.endY - positionY - 1] != originalPixel)
				{
					rightFlag = true;
					rightPos.x = area.endX - positionX - 1;
					rightPos.y = area.startY + area.endY - positionY - 1;
				}

				if (positionY == area.endY - 1 && !rightFlag) //true: positionX = area.endX - 1
				{
					updateArea.startX = area.endX - positionX - 1;
					updateArea.endX = area.endX - positionX;
					updateArea.startY = area.startY;
					updateArea.endY = area.endY;
					editeThePixelArray(originalPixelArray, updateArea);
				}
			}

			if (leftFlag && rightFlag)
			{
				#ifdef DEBUG
					cout << leftPos.x << "," << leftPos.y << endl;
					cout << rightPos.x << "," << rightPos.y << endl;

				#endif
				return;
			}

		}
	}
	throw "Doesn't find a unique pixel";
}



void BMPEditor::writeHeaders(ofstream& file) {
	file.write((const char*)&bmp_header, sizeof(bmp_header));
	file.write((const char*)&dib_header, sizeof(dib_header));
}
BMPEditor::~BMPEditor()
{
}
