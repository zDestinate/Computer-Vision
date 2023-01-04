#include <iostream>
#include <fstream>
#include <string>
#include "ExtractNum.h"
#include "HoughTransform.h"

using namespace std;

int main(int argc, const char* argv[])
{
	if (argc != 3)
	{
		printf("[ERROR] Invalid arguments. Must have 2 arguments.\n");
		return 1;
	}

	//In files
	ifstream inFile(argv[1]);
	if (!inFile)
	{
		printf("[ERROR] Invalid input file. Unable to open file!\n");
		return 1;
	}

	//Out files
	ofstream outFile(argv[2]);
	if (!outFile)
	{
		printf("[ERROR] Invalid out file\n");
		return 1;
	}

	//Extract first line of the input imgfile
	string strLine;
	getline(inFile, strLine);
	int* pFirstLineData = GetNumbers(strLine, ' ');
	printf("[Info] Extracted inFile first line data\n");

	//HoughTransform
	HoughTransform HT(pFirstLineData);
	HT.loadImage(inFile);
	HT.prettyPrint(HT.imgAry, HT.numRows, HT.numCols, outFile, "Original Image");
	HT.buildHoughSpace();
	HT.prettyPrint(HT.CartesianHoughAry, HT.HoughDist, HT.HoughAngle, outFile, "Cartesian Hough Space");
	HT.prettyPrint(HT.PolarHoughAry, HT.HoughDist, HT.HoughAngle, outFile, "Polar Hough Space");
	printf("[Info] HoughTransform completed\n");

	//Close file
	outFile.close();
	printf("[Info] Closed all files\n");
}