#include <iostream>
#include <fstream>
#include <string>
#include "ExtractNum.h"
#include "ImageCompression.h"

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
	ofstream outFile1(argv[2]);
	//Extract the inFile name for skeletonFile and deCompressFile
	string strFileName(argv[1]);
	strFileName = strFileName.substr(0, strFileName.find('.'));
	ofstream skeletonFile(strFileName + "_skeleton.txt");
	ofstream deCompressFile(strFileName + "_deCompressed.txt");
	if (!outFile1 || !skeletonFile || !deCompressFile)
	{
		printf("[ERROR] Invalid out file\n");
		return 1;
	}

	//Extract first line of the input imgfile
	string strLine;
	getline(inFile, strLine);
	int* pFirstLineData = GetNumbers(strLine, ' ');
	printf("[Info] Extracted inFile first line data\n");


	//Image Compression class
	ImageCompression IC(pFirstLineData);
	IC.setZero(IC.ZFAry);
	IC.setZero(IC.skeletonAry);
	IC.loadImage(inFile);
	IC.compute8Distance(IC.ZFAry, outFile1);
	IC.imageCompression(IC.ZFAry, IC.skeletonAry, skeletonFile, outFile1);
	//Close skeleton file
	skeletonFile.close();

	ifstream skeleton_inFile(strFileName + "_skeleton.txt");
	IC.setZero(IC.ZFAry);
	IC.loadSkeleton(skeleton_inFile, IC.ZFAry);
	IC.imageDeCompression(IC.ZFAry, outFile1);
	IC.threshold(IC.ZFAry, deCompressFile);


	//Close output files
	outFile1.close();
	deCompressFile.close();
	printf("[Info] Closed all files\n");
}