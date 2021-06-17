#include "BMP.h"

BMP::BMP(const string& file)
{
    ifstream inputfile(file, ios::binary);
    if (!inputfile)
        throw "Failed to open the file!!!";
    //value the bmp header information
    
    loadBMP_Header(inputfile);

    //load and check the DIB header inforamtion
    loadDIB_Header(inputfile);

    //get the color table information
    loadColorTable(inputfile);

    //get the image data
    loadImageData(inputfile);

    //convert the image data to pixel array
    convertPixels(imageData);

    inputfile.close();
}

//stores bmp header information
//for the data structure alignment, we can only value the struct in this way
void BMP::loadBMP_Header(ifstream& inputfile)
{
    std::vector<char> bmpHeader(BMP_HEADER_SIZE);
    inputfile.read(bmpHeader.data(), bmpHeader.size());
    bmp_header.headerField = *reinterpret_cast<uint16_t*>(&bmpHeader[0]);
    bmp_header.fileSize = *reinterpret_cast<uint32_t*>(&bmpHeader[2]);
    bmp_header.reservedOne = *reinterpret_cast<uint16_t*>(&bmpHeader[6]);
    bmp_header.reservedTwo = *reinterpret_cast<uint16_t*>(&bmpHeader[8]);
    bmp_header.offset = *reinterpret_cast<uint32_t*>(&bmpHeader[10]);

    #ifdef DEBUG
        cout << "headerField: " << bmp_header.headerField << endl;
        cout << "fileSize: " << bmp_header.fileSize << endl;
        cout << "reservedOne: " << bmp_header.reservedOne << endl;
        cout << "reservedTwo: " << bmp_header.reservedTwo << endl;
        cout << "dataOffset: " << bmp_header.offset << endl;
    #endif

    if (bmp_header.headerField != BM)
        throw "The bmp file is not a common Windows format!!";
}

void BMP::loadDIB_Header(ifstream& inputfile)
{
    inputfile.read((char*)&dib_header, sizeof(dib_header));
#ifdef DEBUG
    cout << "headSize: " << dib_header.headerSize << endl;
    cout << "width: " << dib_header.width << endl;
    cout << "height: " << dib_header.height << endl;
    cout << "number of color planes: " << dib_header.colorPlane << endl;
    cout << "depth: " << dib_header.depth << "-bit" << endl;
    cout << "compressionMethod: " << dib_header.compressionMethod << endl;
    cout << "horizontalResolution: " << dib_header.horizontalResolution << endl;
    cout << "verticalResolution: " << dib_header.verticalResolution << endl;
    cout << "sizeOfRawBitmap: " << dib_header.sizeOfRawBitmap << endl;
    cout << "numberOfColors: " << dib_header.numberOfColors << endl;
    cout << "importantColor: " << dib_header.importantColor << endl;
#endif
    if (DIB_HEADER_SIZE != dib_header.headerSize) {
        throw "DIB header size has mistake!!";
    }
    if (bmp_header.offset != BMP_HEADER_SIZE + dib_header.headerSize + dib_header.numberOfColors * RGB32) {
        throw "The loading method doesn't support this image's structure!!";
    }
}

void BMP::loadColorTable(ifstream& inputfile)
{
    if (dib_header.compressionMethod == BI_BITFIELDS || dib_header.compressionMethod == BI_ALPHABITFIELDS)
    {
        throw "The loading method doesn't support this image's structure for the compression method!!";
    }
    UINT colorTableSize = dib_header.numberOfColors * RGB32;
    colorTable = vector<char>(colorTableSize);
    inputfile.read(colorTable.data(), colorTable.size());
}

void BMP::loadImageData(ifstream& inputfile)
{
    imageData = vector<char>(dib_header.sizeOfRawBitmap);
    inputfile.read(imageData.data(), imageData.size());
    if (dib_header.depth != BITS_PER_PIXEL)
    {
        throw "The loading method doesn't support this image's structure since the image is not 8-bits!!";
    }
}

void BMP::convertPixels(vector<char> imageData)
{
    UINT padRowSize = 0;
    if (dib_header.width * dib_header.height == dib_header.sizeOfRawBitmap) {
        padRowSize = 0;
    }
    else {
        padRowSize = (dib_header.width % 4 == 0) ? 0 : 4 - (dib_header.width % 4);
    }
    UINT pixelWidth = dib_header.width + padRowSize;
    UINT pixelHeight = dib_header.height;

    pixelsArray = vector<vector<int> >(pixelWidth, vector<int>(pixelHeight));

    for (int index = 0; index < dib_header.sizeOfRawBitmap; index++) {

        UINT positionX = index % pixelWidth;
        UINT positionY = index / pixelHeight;
        pixelsArray[positionX][positionY] = int(imageData[index] & 0xff);
    }

}


BMP ::~BMP ()
{
}

BMP_Header BMP::GetBMP_Header()
{
    return bmp_header;
}

DIB_Header BMP::GetDIB_Header()
{
    return dib_header;
}

vector<char> BMP::GetColorTable()
{
    return colorTable;
}

vector<char> BMP::GetImageData()
{
    return imageData;
}

vector<vector<int> > BMP::GetPixelArray() 
{
   return pixelsArray;
}
