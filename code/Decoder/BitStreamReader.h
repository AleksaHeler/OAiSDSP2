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
	int readData(char data[], int size);

	//decode methods
	void YUV420toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int x, int y, uchar rgbImg[]);

	void GenerateDCTmatrix(double* DCTKernel, int order);
	void IDCT(const short input[], uchar output[], int N, double* DCTKernel);
	void doIDCTDecompression(uchar Y_buff[], short* short_buff, int xSize, int ySize, int N, uchar* input2, int xSize2, int ySize2);

	void extendBorders(uchar* input, int xSize, int ySize, int N, uchar** p_output, int* newXSize, int* newYSize);
	void cropImage(uchar* input, int xSize, int ySize, uchar* output, int newXSize, int newYSize);

	void performInverseDCTQuantization(short* dctCoeffs);

	void zeroRunLengthDecode(std::vector<short>& v, short* output, int xSize, int ySize);

	static uchar quantizationMatrix[64];


public:
	// initialize reader
	explicit BitStreamReader(QFile& input) : inputFile(input) {}

	// Entry function
	// Decodes image
	// Expcted output is image in RGB888 format (output), 
	// and image size (xSize and ySize)
	bool decode(uchar* &output, int &xSize, int &ySize);
};
