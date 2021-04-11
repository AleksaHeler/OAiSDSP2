#include <cstdlib> 
#include <iostream>
#include <string>
#include <vector>

#include <QString>
#include <QImage>
#include <QFile>
#include <QDebug>

using namespace std;

int main()
{
	//place path of executable files
	string path_exe = "C:\\Users\\Radomir-PC\\Desktop\\dsp_domaci\\code\\x64\\Debug";

	//place path of image files (needs to be in bmp encoded and 24bit color)
	string path_image = "C:\\Users\\Radomir-PC\\Desktop\\dsp_domaci\\images";

	//if Encoder.exe and Decoder.exe are in the path_exe then dont change this, else provide full path to exe files
	string encode = path_exe + "\\Encoder.exe";
	string decode = path_exe + "\\Decoder.exe";

	//place name of images (without extension)
	vector<string> images = { 
								"baboon", 
								"clock",
								"czv",
								"image1",
								"lena",
								"sample",
								"bird",
								"blackHole",
								"butterfly",
								"car"
							};

	//nothing should be chnaged from here
	//".bmp" will be extension for input and output images
	//".rtrk" will be extension for encoded images
	for (auto it = images.begin(); it != images.end(); it++)
	{
		string encode_arguments = path_image + "\\"+ *it + ".bmp" + " " + path_image + "\\" + *it + ".rtrk";
		string encode_command = encode + " " + encode_arguments;

		string decode_arguments = path_image + "\\" + *it + ".rtrk" + " " + path_image + "\\" + *it + "_final" + ".bmp";
		string decode_command = decode + " " + decode_arguments;

		cout << "Encoding " << "[ " << *it << " ]" << ":" << endl;
		system(encode_command.c_str());

		cout << "Decoding " << "[ " << *it << " ]" << ":" << endl;
		system(decode_command.c_str());	
		cout << "-----------------------------------" << endl << endl;
	}

	cout << "COMPRESSION FINISHED\n-----------------------------------" << endl << endl;

	cout << "\nMSE:" << endl;
	string original_filename, final_filename;
	for (auto it = images.begin(); it != images.end(); it++)
	{
		original_filename = path_image + "\\" + *it + ".bmp";
		final_filename = path_image + "\\" + *it + "_final" + ".bmp";

		QString originalFileName(original_filename.c_str());
		QString finalFileName(final_filename.c_str());

		QImage originalImage = QImage(originalFileName).convertToFormat(QImage::Format_RGB888);
		if (originalImage.isNull())
		{
			cout << "Unable to open image " << original_filename << endl;
		}

		QImage finalImage = QImage(finalFileName).convertToFormat(QImage::Format_RGB888);
		if (finalImage.isNull())
		{
			cout << "Unable to open image " << final_filename << endl;
		}

		uchar oR, oG, oB, fR, fG, fB;
		double yMSE = 0;
		double rMSE = 0;
		double gMSE = 0;
		double bMSE = 0;
		uchar* original_buff = originalImage.bits();
		uchar* final_buff = finalImage.bits();
		int xSize = originalImage.width();
		int ySize = originalImage.height();
		for (int i = 0; i < xSize; i++)
		{
			for (int j = 0; j < ySize; j++)
			{
				oR = original_buff[j*xSize * 3 + i * 3];
				fR = final_buff[j*xSize * 3 + i * 3];

				oG = original_buff[j*xSize * 3 + i * 3 + 1];
				fG = final_buff[j*xSize * 3 + i * 3 + 1];

				oB = original_buff[j*xSize * 3 + i * 3 + 2];
				fB = final_buff[j*xSize * 3 + i * 3 + 2];

				rMSE += (oR - fR) * (oR - fR);
				gMSE += (oG - fG) * (oG - fG);
				bMSE += (oB - fB) * (oB - fB);

				yMSE += ( (0.299*oR + 0.587*oG + 0.114*oB) - (0.299*fR + 0.587*fG + 0.114*fB) )
					 * ( (0.299*oR + 0.587*oG + 0.114*oB) - (0.299*fR + 0.587*fG + 0.114*fB) );
			}
		}

		yMSE /= xSize * ySize;
		rMSE /= xSize * ySize;
		gMSE /= xSize * ySize;
		bMSE /= xSize * ySize;

		cout << *it << ".bmp Y MSE = " << yMSE << endl;
		cout << *it << ".bmp RED MSE = " << rMSE << endl;
		cout << *it << ".bmp GREEN MSE = " << gMSE << endl;
		cout << *it << ".bmp BLUE MSE = " << bMSE << endl;
		cout << "-----------------------------------" << endl << endl;
	}

	system("pause");

    return 0;
}