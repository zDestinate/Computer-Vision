#include <iostream>
#include <fstream>
#include <string>
#include "ExtractNum.h"
#include "Morphology.h"

using namespace std;

int main(int argc, const char* argv[])
{
	if (argc != 8)
	{
		printf("[ERROR] Invalid arguments. Must have 7 arguments.\n");
		return 1;
	}

	//In files
	ifstream imgFile(argv[1]);
	if (!imgFile)
	{
		printf("[ERROR] Invalid input file. Unable to open file!\n");
		return 1;
	}

	ifstream structFile(argv[2]);
	if (!structFile)
	{
		printf("[ERROR] Invalid input file 2. Unable to open file!\n");
		return 1;
	}

	//Out files
	ofstream dilateOutFile(argv[3]);
	ofstream erodeOutFile(argv[4]);
	ofstream closingOutFile(argv[5]);
	ofstream openingOutFile(argv[6]);
	ofstream prettyPrintFile(argv[7]);
	if (!dilateOutFile && !erodeOutFile && !closingOutFile && !openingOutFile && !prettyPrintFile)
	{
		printf("[ERROR] Invalid out file\n");
		return 1;
	}


	//Extract first line of the input imgfile
	string strLine;
	getline(imgFile, strLine);
	int* pImgFirstLineData = GetNumbers(strLine, ' ');
	printf("[Info] Extracted imgFile first line data\n");
	
	//Extract first line of the input structFile
	getline(structFile, strLine);
	int* pStructFirstLineData = GetNumbers(strLine, ' ');
	printf("[Info] Extracted structFile first line data\n");
	//Extract origin
	getline(structFile, strLine);
	int* pStructOriginData = GetNumbers(strLine, ' ');
	printf("[Info] Extracted origin data\n");


	//Morphology class
	Morphology MP(pImgFirstLineData, pStructFirstLineData, pStructOriginData);
	//Remove pointers (No longer need since we store it in the class now)
	delete[] pImgFirstLineData, pStructFirstLineData, pStructOriginData;

	MP.zero2DAry(MP.zeroFramedAry, MP.rowSize, MP.colSize);
	MP.loadImg(imgFile, MP.zeroFramedAry);
	MP.prettyPrint(MP.zeroFramedAry, prettyPrintFile, MP.rowSize, MP.colSize, MP.imgMin, MP.imgMax, "Original Image (With frame)");

	MP.zero2DAry(MP.structAry, MP.numStructRows, MP.numStructCols);
	MP.loadstruct(structFile, MP.structAry);
	MP.prettyPrint(MP.structAry, prettyPrintFile, MP.numStructRows, MP.numStructCols, MP.structMin, MP.structMax, "Original Structure Element", true);

	MP.zero2DAry(MP.morphAry, MP.rowSize, MP.colSize);
	MP.ComputeDilation(MP.zeroFramedAry, MP.morphAry);
	MP.AryToFile(MP.morphAry, dilateOutFile);
	MP.prettyPrint(MP.morphAry, prettyPrintFile, MP.rowSize, MP.colSize, MP.imgMin, MP.imgMax, "Dilation (With frame)");
	printf("[Morphology] Dilation Completed\n");

	MP.zero2DAry(MP.morphAry, MP.rowSize, MP.colSize);
	MP.ComputeErosion(MP.zeroFramedAry, MP.morphAry);
	MP.AryToFile(MP.morphAry, erodeOutFile);
	MP.prettyPrint(MP.morphAry, prettyPrintFile, MP.rowSize, MP.colSize, MP.imgMin, MP.imgMax, "Erosion (With frame)");
	printf("[Morphology] Erosion Completed\n");

	MP.zero2DAry(MP.morphAry, MP.rowSize, MP.colSize);
	MP.ComputeOpening(MP.zeroFramedAry, MP.morphAry, MP.tempAry);
	MP.AryToFile(MP.morphAry, openingOutFile);
	MP.prettyPrint(MP.morphAry, prettyPrintFile, MP.rowSize, MP.colSize, MP.imgMin, MP.imgMax, "Opening (With frame)");
	printf("[Morphology] Opening Completed\n");

	MP.zero2DAry(MP.morphAry, MP.rowSize, MP.colSize);
	MP.ComputeClosing(MP.zeroFramedAry, MP.morphAry, MP.tempAry);
	MP.AryToFile(MP.morphAry, closingOutFile);
	MP.prettyPrint(MP.morphAry, prettyPrintFile, MP.rowSize, MP.colSize, MP.imgMin, MP.imgMax, "Closing (With frame)");
	printf("[Morphology] Closing Completed\n");



	//Close all output files
	dilateOutFile.close();
	erodeOutFile.close();
	closingOutFile.close();
	openingOutFile.close();
	prettyPrintFile.close();
	printf("[Info] Closed all files\n");
}