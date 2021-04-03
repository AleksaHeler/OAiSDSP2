#include <QtCore/QCoreApplication>
#include <QString>
#include <QImage>
#include <QFile>
#include <QDebug>
#include <iostream>
#include "BitStreamReader.h"

using namespace std;


int main(int argc, char *argv[])
{

	if (argc != 3)
	{
		qCritical() << "Error: Invalid number of arguments!" << endl;
		qCritical() << "Command format: Decoder.exe [input_file] [output_file]";
		return -1;
	}
		
	QString inputFileName(argv[1]);
	QString outputFileName(argv[2]);

	QFile& inputFile = QFile(inputFileName);
	if (!inputFile.open(QIODevice::ReadOnly))
	{
		qCritical() << "Error: Unable to open input file " << inputFileName << endl;
		return -1;
	}

	uchar* rgbImageData;
	int xSize = 0;
	int ySize = 0;

	BitStreamReader inputReader = BitStreamReader(inputFile);

	bool decodeSucsess = inputReader.decode(rgbImageData, xSize, ySize);
	
	// Close input file
	inputFile.close();

	if (!decodeSucsess)
	{
		qCritical() << "Error: Decode failed: ";
		return -1;
	}

	QImage outputImage = QImage(rgbImageData, xSize, ySize, QImage::Format_RGB888);
	outputImage.save(outputFileName);

	qInfo() << "Decoding finished!";
	
	return 0;
}

