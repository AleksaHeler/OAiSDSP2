#pragma once
#include <QDebug>
#include <QFile>


static const char FILE_HEADER[] = { 'R', 'T', 'R', 'K' };
static const int HEADER_SIZE = sizeof(FILE_HEADER);

class BitStreamReader
{
private:
	QFile& inputFile;

	// read header
	// return true if header maches FILE_HEADER
	bool readHeader();

	// read image info (width and height)
	bool readImageInfo(int& width, int& height);

	// Helper function
	// Skip bytes in input stream
	bool skip(int bytes);
	
	// read a single byte from input stream
	uchar readByte();

	// read a short integer from input stream 
	ushort readShort();

	// read N bytes from input stream
	// returns number of bytes read (smaller of size if EOF reached)
	int readData(uchar data[], int size);

public:
	// initialize reader
	explicit BitStreamReader(QFile& input) : inputFile(input) {}

	// Entry function
	// Decodes image
	// Expcted output is image in RGB888 format (output), 
	// and image size (xSize and ySize)
	bool decode(uchar* &output, int &xSize, int &ySize);
};
