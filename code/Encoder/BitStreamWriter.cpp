#pragma once
#include "BitStreamWriter.h"
#include "HuffmanEnc.h"

#define PI 3.14159265359

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
int BitStreamWriter::writeData(char data[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (outputFile.putChar(data[i]) == false)
			break;
	}
	return size;
}


/* 8x8 Quantization matrix */
uchar BitStreamWriter::LuminanceQuantizationMatrix[64] =
{
	2, 2, 2, 2, 3, 5, 7, 8,
	2, 2, 2, 3, 4, 8, 8, 7,
	2, 2, 2, 3, 5, 8, 9, 7,
	2, 3, 3, 4, 7, 11, 10, 8,
	3, 3, 5, 7, 9, 14, 13, 10,
	3, 5, 7, 8, 11, 13, 15, 12,
	7, 8, 10, 11, 13, 16, 15, 13,
	9, 12, 12, 13, 29, 13, 13, 13
};

uchar BitStreamWriter::ChrominanceQuantizationMatrix[64] =
{
	3, 3, 5, 6, 25, 25, 25, 25,
	3, 3, 4, 9, 25, 25, 25, 25,
	3, 4, 7, 25, 25, 25, 25, 25,
	6, 9, 25, 25, 25, 25, 25, 25,
	25, 25, 25, 25, 25, 25, 25, 25,
	25, 25, 25, 25, 25, 25, 25, 25,
	25, 25, 25, 25, 25, 25, 25, 25,
	25, 25, 25, 25, 25, 25, 25, 25
};

void BitStreamWriter::RGBtoYUV420(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[])
{
	uchar R, G, B;
	double U, V;
	for (int i = 0; i<x; i += 2)
	{
		for (int j = 0; j<y; j += 2)
		{
			R = rgbImg[j * 3 * x + i * 3];
			G = rgbImg[j * 3 * x + i * 3 + 1];
			B = rgbImg[j * 3 * x + i * 3 + 2];

			Y_buff[j*x + i] = 0.299*R + 0.587*G + 0.114*B;
			U = (-0.14713*R - 0.28886*G + 0.436*B);
			V = (R*0.615 - 0.51499*G - 0.10001*B);

			R = rgbImg[j * 3 * x + (i + 1) * 3];
			G = rgbImg[j * 3 * x + (i + 1) * 3 + 1];
			B = rgbImg[j * 3 * x + (i + 1) * 3 + 2];

			Y_buff[j*x + (i + 1)] = 0.299*R + 0.587*G + 0.114*B;
			U += (-0.14713*R - 0.28886*G + 0.436*B);
			V += (R*0.615 - 0.51499*G - 0.10001*B);

			R = rgbImg[(j + 1) * 3 * x + i * 3];
			G = rgbImg[(j + 1) * 3 * x + i * 3 + 1];
			B = rgbImg[(j + 1) * 3 * x + i * 3 + 2];

			Y_buff[(j + 1)*x + i] = 0.299*R + 0.587*G + 0.114*B;
			U += (-0.14713*R - 0.28886*G + 0.436*B);
			V += (R*0.615 - 0.51499*G - 0.10001*B);

			R = rgbImg[(j + 1) * 3 * x + (i + 1) * 3];
			G = rgbImg[(j + 1) * 3 * x + (i + 1) * 3 + 1];
			B = rgbImg[(j + 1) * 3 * x + (i + 1) * 3 + 2];

			Y_buff[(j + 1)*x + (i + 1)] = 0.299*R + 0.587*G + 0.114*B;
			U += (-0.14713*R - 0.28886*G + 0.436*B);
			V += (R*0.615 - 0.51499*G - 0.10001*B);

			U_buff[(j / 2)*x / 2 + i / 2] = U / 4;
			V_buff[(j / 2)*x / 2 + i / 2] = V / 4;
		}
	}
}

void BitStreamWriter::GenerateDCTmatrix(double* DCTKernel, int order)
{
	int i, j;
	int N;
	N = order;
	double alpha;
	double denominator;
	for (j = 0; j <= N - 1; j++)
	{
		DCTKernel[0, j] = sqrt(1 / (double)N);
	}
	alpha = sqrt(2 / (double)N);
	denominator = (double)2 * N;
	for (j = 0; j <= N - 1; j++)
		for (i = 1; i <= N - 1; i++)
		{
			DCTKernel[i*N + j] = alpha * cos(((2 * j + 1) * i * PI) / denominator);
		}
}

void BitStreamWriter::DCT(const uchar input[], short output[], int N, double* DCTKernel)
{
	double* temp = new double[N*N];
	double* DCTCoefficients = new double[N*N];

	double sum;
	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + DCTKernel[i*N + k] * (input[k*N + j] - 128.0);
			}
			temp[i*N + j] = sum;
		}
	}

	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + temp[i*N + k] * DCTKernel[j*N + k];
			}
			DCTCoefficients[i*N + j] = sum;
		}
	}

	for (int i = 0; i < N*N; i++)
	{
		output[i] = floor(DCTCoefficients[i] + 0.5);
	}

	delete[] temp;
	delete[] DCTCoefficients;

	return;
}

void BitStreamWriter::DCT(const char input[], short output[], int N, double* DCTKernel)
{
	double* temp = new double[N*N];
	double* DCTCoefficients = new double[N*N];

	double sum;
	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + DCTKernel[i*N + k] * (input[k*N + j]);
			}
			temp[i*N + j] = sum;
		}
	}

	for (int i = 0; i <= N - 1; i++)
	{
		for (int j = 0; j <= N - 1; j++)
		{
			sum = 0;
			for (int k = 0; k <= N - 1; k++)
			{
				sum = sum + temp[i*N + k] * DCTKernel[j*N + k];
			}
			DCTCoefficients[i*N + j] = sum;
		}
	}

	for (int i = 0; i < N*N; i++)
	{
		output[i] = floor(DCTCoefficients[i] + 0.5);
	}

	delete[] temp;
	delete[] DCTCoefficients;

	return;
}

void BitStreamWriter::doOnDCTCompression(short* output, int xSize, int ySize, int N, uchar* input2, int xSize2, int ySize2)
{	
	/* Create NxN buffer for one input block */
	uchar* inBlock = new uchar[N*N];

	/* Generate DCT kernel */
	double* DCTKernel = new double[N*N];
	GenerateDCTmatrix(DCTKernel, N);

	/* Create buffer for DCT coefficients */
	short* dctCoeffs = new short[N*N];

	int cnt = 0;
	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			/* Fill input block buffer */
			for (int j = 0; j<N; j++)
			{
				for (int i = 0; i<N; i++)
				{
					inBlock[j*N + i] = input2[(N*y + j)*(xSize2)+N*x + i];
				}
			}

			/* Invoke DCT */
			DCT(inBlock, dctCoeffs, N, DCTKernel);

			performDCTQuantization(dctCoeffs, 0); //luminance quantization table

			doZigZag(dctCoeffs, 8, 8*8 * 0.7);

			
			for (int i = 0; i < 64; i++)
			{	
				output[cnt * 64 + i] = dctCoeffs[i];
			}

			cnt++;
		}
	}

	/* Delete used memory buffers coefficients */
	delete[] dctCoeffs;
	delete[] inBlock;
	delete[] DCTKernel;
}

void BitStreamWriter::doOnDCTCompression(short* output, int xSize, int ySize, int N, char* input2, int xSize2, int ySize2)
{
	/* Create NxN buffer for one input block */
	char* inBlock = new char[N*N];

	/* Generate DCT kernel */
	double* DCTKernel = new double[N*N];
	GenerateDCTmatrix(DCTKernel, N);

	/* Create buffer for DCT coefficients */
	short* dctCoeffs = new short[N*N];

	int cnt = 0;
	for (int y = 0; y < ySize2 / N; y++)
	{
		for (int x = 0; x < xSize2 / N; x++)
		{
			/* Fill input block buffer */
			for (int j = 0; j<N; j++)
			{
				for (int i = 0; i<N; i++)
				{
					inBlock[j*N + i] = input2[(N*y + j)*(xSize2)+N*x + i];
				}
			}

			/* Invoke DCT */
			DCT(inBlock, dctCoeffs, N, DCTKernel);

			performDCTQuantization(dctCoeffs, 1); //chrominance quantization table

			doZigZag(dctCoeffs, 8, 8 * 8 * 0.85);


			for (int i = 0; i < 64; i++)
			{
				output[cnt * 64 + i] = dctCoeffs[i];
			}

			cnt++;
		}
	}

	/* Delete used memory buffers coefficients */
	delete[] dctCoeffs;
	delete[] inBlock;
	delete[] DCTKernel;
}

void BitStreamWriter::extendBorders(uchar* input, int xSize, int ySize, int N, uchar** p_output, int* newXSize, int* newYSize)
{
	int deltaX = N - xSize%N;
	int deltaY = N - ySize%N;

	*newXSize = xSize + deltaX;
	*newYSize = ySize + deltaY;

	uchar* output = new uchar[(xSize + deltaX)*(ySize + deltaY)];

	for (int i = 0; i<ySize; i++)
	{
		memcpy(&output[i*(xSize + deltaX)], &input[i*(xSize)], xSize);
		if (deltaX != 0)
		{
			memset(&output[i*(xSize + deltaX) + xSize], output[i*(xSize + deltaX) + xSize - 1], deltaX);
		}
	}

	for (int i = 0; i<deltaY; i++)
	{
		memcpy(&output[(i + ySize)*(xSize + deltaX)], &output[(ySize)*(xSize + deltaX)], xSize + deltaX);
	}

	*p_output = output;
}


void BitStreamWriter::extendBorders(char* input, int xSize, int ySize, int N, char** p_output, int* newXSize, int* newYSize)
{
	int deltaX = N - xSize%N;
	int deltaY = N - ySize%N;

	*newXSize = xSize + deltaX;
	*newYSize = ySize + deltaY;

	char* output = new char[(xSize + deltaX)*(ySize + deltaY)];

	for (int i = 0; i<ySize; i++)
	{
		memcpy(&output[i*(xSize + deltaX)], &input[i*(xSize)], xSize);
		if (deltaX != 0)
		{
			memset(&output[i*(xSize + deltaX) + xSize], output[i*(xSize + deltaX) + xSize - 1], deltaX);
		}
	}

	for (int i = 0; i<deltaY; i++)
	{
		memcpy(&output[(i + ySize)*(xSize + deltaX)], &output[(ySize)*(xSize + deltaX)], xSize + deltaX);
	}

	*p_output = output;
}

void BitStreamWriter::cropImage(short* input, int xSize, int ySize, short* output, int newXSize, int newYSize)
{
	for (int i = 0; i<newYSize; i++)
	{
		memcpy(&output[i*(newXSize)], &input[i*(xSize)], newXSize);
	}
}

void BitStreamWriter::performDCTQuantization(short* dctCoeffs, int quantizationMatrix)
{
	uchar* matrix;
	if (quantizationMatrix == 0)
		matrix = LuminanceQuantizationMatrix;
	else
		matrix = ChrominanceQuantizationMatrix;

	const int N = 8;
	for (int j = 0; j<N; j++)
	{
		for (int i = 0; i<N; i++)
		{
			dctCoeffs[j*N + i] = floor(dctCoeffs[j*N + i] / (matrix[j*N + i]) + 0.5);
		}
	}
}

void BitStreamWriter::doZigZag(short block[], int N, int B)
{
	int currDiagonalWidth = 1;
	int i;
	int row, col;
	int currentNumber = 0;
	while (currDiagonalWidth<N)
	{
		for (i = 0; i<currDiagonalWidth; i++)
		{
			if (currDiagonalWidth % 2 == 1)
			{
				row = currDiagonalWidth - i - 1;
				col = i;
			}
			else
			{
				row = i;
				col = currDiagonalWidth - i - 1;
			}
			
			if (currentNumber > B) {
				block[row*N + col] = 0;
			}
			currentNumber++;
		}
		currDiagonalWidth++;
	}
	while (currDiagonalWidth> 0)
	{
		for (i = currDiagonalWidth; i> 0; i--)
		{
			if (currDiagonalWidth % 2 == 1)
			{
				row = N - currDiagonalWidth + i - 1;
				col = N - i;
			}
			else
			{
				row = N - i;
				col = N - currDiagonalWidth + i - 1;
			}
			
			if (currentNumber > B)
				block[row*N + col] = 0;

			currentNumber++;
		}
		
		currDiagonalWidth--;
	}
}

void BitStreamWriter::zeroRunLengthEncode(short* block, std::vector<short>* v, int xSize, int ySize)
{
	int eob = 0;
	for (int i = xSize*ySize - 1; i >= 0; i--) //end of block
	{
		if (block[i] == 0)
			eob++;
		else
			break;
	}

	short zero_cnt = 0;
	for (int i = 0; i < xSize *ySize - eob; i++)
	{
		if (block[i] == 0)
			zero_cnt++;
		else
		{
			v->push_back(zero_cnt); //numbers of zeros
			v->push_back(block[i]); //numbers after zeros

			zero_cnt = 0;
		}
	}

	//end of block
	v->push_back(0);
	v->push_back(0);
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

	//RGB to YUV420
	uchar* Y_buff = new uchar[xSize*ySize];
	char* U_buff = new char[xSize*ySize / 4];
	char* V_buff = new char[xSize*ySize / 4];
	RGBtoYUV420(input, xSize, ySize, Y_buff, U_buff, V_buff);

	//DCT, quantization, zigzag for Y component
	int xSize2, ySize2;
	uchar* input2;
	extendBorders(Y_buff, xSize, ySize, 8, &input2, &xSize2, &ySize2);
	short* dct_coeffs = new short[xSize2 * ySize2];
	doOnDCTCompression(dct_coeffs, xSize, ySize, 8, input2, xSize2, ySize2);
	//run length coding on Y component
	std::vector<short> v_Y;
	zeroRunLengthEncode(dct_coeffs, &v_Y, xSize2, ySize2);

	//extending borders and creating new inputs
	int xSize3, ySize3;
	int xSize4, ySize4;
	char* input3;
	char* input4;
	extendBorders(U_buff, (int)(xSize / 2), (int)(ySize / 2), 8, &input3, &xSize3, &ySize3);
	extendBorders(V_buff, (int)(xSize / 2), (int)(ySize / 2), 8, &input4, &xSize4, &ySize4);

	//DCT, quantization, zigzag for U component
	short* dct_coeffs2 = new short[xSize3 * ySize3];
	doOnDCTCompression(dct_coeffs2, (int)(xSize / 2), (int)(ySize / 2), 8, input3, xSize3, ySize3);
	//run length coding on U component
	std::vector<short> v_U;
	zeroRunLengthEncode(dct_coeffs2, &v_U, xSize3, ySize3);

	//DCT, quantization, zigzag for V component
	short* dct_coeffs3 = new short[xSize4 * ySize4];
	doOnDCTCompression(dct_coeffs3, (int)(xSize / 2), (int)(xSize / 2), 8, input4, xSize4, ySize4);
	//run length coding on V component
	std::vector<short> v_V;
	zeroRunLengthEncode(dct_coeffs3, &v_V, xSize4, ySize4);


	//writing in huffman code format
	bool ret_val = doHuffmanEncoding(outputFile, v_Y, v_U, v_V);
	if (ret_val)
		qCritical() << "Huffman encoding completed successfully" << endl;
	else
		qCritical() << "Error in HuffmanEncoding" << endl;

	delete[] dct_coeffs;
	delete[] dct_coeffs2;

	delete[] Y_buff;
	delete[] U_buff;
	delete[] V_buff;

	return ret_val;
}