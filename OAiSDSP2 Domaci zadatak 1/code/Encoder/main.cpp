#include <QtCore/QCoreApplication>
#include <QString>
#include <QImage>
#include <QFile>
#include <QDebug>
#include <iostream>
#include "BitStreamWriter.h"

using namespace std;


int main(int argc, char *argv[])
{

	if (argc != 3)
	{
		qCritical() << "Error: Invalid number of arguments!" << endl;
		qCritical() << "Command format: Encoder.exe [input_file] [output_file]";
		return -1;
	}
		
	QString inputFileName(argv[1]);
	QString outputFileName(argv[2]);

	QImage inputImage = QImage(inputFileName).convertToFormat(QImage::Format_RGB888);
	if (inputImage.isNull())
	{
		qCritical() << "Error: Unable to open input image: " << inputFileName << endl;
		return -1;
	}

	QFile& outputFile = QFile(outputFileName);
	if (!outputFile.open(QIODevice::WriteOnly))
	{
		qCritical() << "Error: Unable to open output file " << outputFileName << endl;
		return -1;
	}

	// Create outputWriter object
	BitStreamWriter outputWriter = BitStreamWriter(outputFile);


	// Invoke encoder function
	bool encodeSucsess = outputWriter.encode(inputImage.bits(), inputImage.width(), inputImage.height());

	// Close output file
	outputFile.close();

	if (!encodeSucsess)
	{
		qCritical() << "Error: Encode failed: ";
		return -1;
	}

	qInfo() << "Compression finished!";
	
	return 0;
}

