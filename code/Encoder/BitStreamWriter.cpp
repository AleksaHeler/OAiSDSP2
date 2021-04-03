#pragma once
#include "BitStreamWriter.h"

// write a File header
bool BitStreamWriter::writeHeader()
{
	for (int i = 0; i < HEADER_SIZE; i++)
	{
		if (outputFile.putChar(FILE_HEADER[i]) == false)
			return false;
	}
	return true;
}

// write image info
bool BitStreamWriter::writeImageInfo(int width, int height)
{	
	if (writeShort((ushort)width) == false)
		return false;
	if (writeShort((ushort)height) == false)
		return false;
	return true;
}


// write a single byte
bool BitStreamWriter::writeByte(uchar oneByte)
{
	if (outputFile.putChar(oneByte) == false)
		return false;

	return true;
}

// write a short integer
bool BitStreamWriter::writeShort(ushort data)
{
	if (outputFile.putChar(*(((uchar*)&data) + 1)) == false)
		return false;

	if (outputFile.putChar(*((uchar*)&data)) == false)
		return false;

	return true;
}

// write an array of bytes
int BitStreamWriter::writeData(uchar data[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (outputFile.putChar(data[i]) == false)
			break;
	}
	return size;
}

bool BitStreamWriter::encode(uchar input[], int xSize, int ySize)
{
	// Write image header
	bool ret = writeHeader();
	
	if(!ret)
		return false;

	// Write image info
	ret = writeImageInfo(xSize, ySize);

	if (!ret)
		return false;



	// THIS IS THE PART OF CODE WHERE ENCODING SHOULD HAPPEN
	// FOR NOW, ONLY WRITE RGB IMAGE (RAW)



	// Perform image encoding and write the compressed data
	int size = writeData(input, xSize* ySize * 3);

	if (size != xSize * ySize * 3)
		return false;

	return true;

}
