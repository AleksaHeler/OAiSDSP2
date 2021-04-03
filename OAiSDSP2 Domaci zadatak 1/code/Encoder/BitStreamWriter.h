#pragma once
#include <QDebug>
#include <QFile>

static const char FILE_HEADER[] = { 'R', 'T', 'R', 'K' };
static const int HEADER_SIZE = sizeof(FILE_HEADER);


class BitStreamWriter
{
private:
	QFile& outputFile;

	// write a File Header
	bool writeHeader();

	// write image info
	bool writeImageInfo(int width, int height);

	// write a single byte
	bool writeByte(uchar oneByte);

	// write a short integer
	bool writeShort(ushort data);

	// write an array of bytes
	int writeData(uchar data[], int size);

public:
	// initialize writer
	explicit BitStreamWriter(QFile& output) : outputFile(output) {}

	// Entry function
	// Encodes image and writes to output stream
	// Expcted input is raw image in RGB888 format (input), 
	// and image size (xSize and ySize)
	bool encode(uchar input[], int xSize, int ySize);
};
