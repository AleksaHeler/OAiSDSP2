#pragma once
#include "BitStreamReader.h"
#include "QString"
#include "QDebug"

// Read and check header
bool BitStreamReader::readHeader()
{
	char* header = new char[HEADER_SIZE];

	// Read header
	readData((uchar*)header, HEADER_SIZE);
	
	// Check header content
	for (int i = 0; i < HEADER_SIZE; i++)
	{
		if (header[i] != FILE_HEADER[i])
			return false;
	}

	delete[] header;

	return true;

}

// Read image size
bool BitStreamReader::readImageInfo(int& width, int& height)
{
	width = readShort();
	height = readShort();

	return true;
}

// Decode image
// Expcted output is image in RGB888 format
bool BitStreamReader::decode(uchar* &output, int &xSize, int &ySize)
{
	// Read and check header
	if (!readHeader())
	{
		qCritical() << "ERROR::File Header corrupted.";
		return false;
	}

	// Get image size
	readImageInfo(xSize, ySize);

	// Create output image buffer
	output = new uchar[xSize*ySize * 3];





	// THIS IS THE PART OF CODE WHERE DECODING SHOULD HAPPEN
	// FOR NOW, ONLY READ RGB IMAGE (RAW)




	// Read data from input file
	int bytesCount = readData(output, xSize*ySize*3);
	
	// if number of bytes does bit differs from expected, report an error
	if (bytesCount != (xSize * ySize * 3))
	{
		qCritical() << "ERROR: Reached EOF before reading entire image.";
		return false;
	}
	
	return true;
}


uchar BitStreamReader::readByte()
{
	uchar data;
	inputFile.getChar((char*)&data);
	return data;
}


ushort BitStreamReader::readShort()
{
	ushort data;
	inputFile.getChar(((char*)&data) + 1);
	inputFile.getChar((char*)&data);

	return data;
}

int BitStreamReader::readData(uchar data[], int size)
{
	return inputFile.read((char*)data, size);
}

bool BitStreamReader::skip(int bytes)
{
	inputFile.seek(inputFile.pos() + bytes);
	return true;
}