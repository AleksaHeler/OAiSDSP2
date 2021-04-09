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
	int writeData(char data[], int size);


	//methods used for encoding
	void RGBtoYUV420(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[]);

	void GenerateDCTmatrix(double* DCTKernel, int order);
	void DCT(const uchar input[], short output[], int N, double* DCTKernel); 
	void doOnDCTCompression(uchar input[], short* output, int xSize, int ySize, int N, uchar* input2, int xSize2, int ySize2);

	void extendBorders(uchar* input, int xSize, int ySize, int N, uchar** p_output, int* newXSize, int* newYSize);
	void cropImage(short* input, int xSize, int ySize, short* output, int newXSize, int newYSize);

	void performDCTQuantization(short* dctCoeffs);

	void doZigZag(short block[], int N, int B); 

	void zeroRunLengthEncode(short* block, std::vector<short>* v, int xSize, int ySize);

	/* 8x8 Quantization matrix */
	static uchar quantizationMatrix[64];

public:
	// initialize writer
	explicit BitStreamWriter(QFile& output) : outputFile(output) {}

	// Entry function
	// Encodes image and writes to output stream
	// Expcted input is raw image in RGB888 format (input), 
	// and image size (xSize and ySize)
	bool encode(uchar input[], int xSize, int ySize);
};
