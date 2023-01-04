#include <iostream>
#include <fstream>
#include <string>
#include "ExtractNum.h"
#include "ChainCode.h"

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
	ofstream debugFile(argv[2]);
	//Extract the inFile name for skeletonFile and deCompressFile
	string strFileName(argv[1]);
	strFileName = strFileName.substr(0, strFileName.find('.'));
	ofstream chainCodeFile(strFileName + "_chainCodeFile");
	ofstream deCompressedFile(strFileName + "_deCompressedFile");
	if (!debugFile || !chainCodeFile || !deCompressedFile)
	{
		printf("[ERROR] Invalid output files\n");
		return 1;
	}

	//Extract first line of the input imgfile
	string strLine;
	getline(inFile, strLine);
	int* pFirstLineData = GetNumbers(strLine, ' ');
	printf("[Info] Extracted inFile first line data\n");

	//ChainCode
	ChainCode CC(pFirstLineData);
	CC.loadImage(inFile);
	CC.reformatPrettyPrint(CC.imgAry, debugFile, "Original Image");
	CC.getChainCode(CC.imgAry, chainCodeFile);
	chainCodeFile.close();

	//Now open chaincode file
	ifstream inChainCodeFile(strFileName + "_chainCodeFile");
	CC.deCompression(CC.reconstructAry, inChainCodeFile, debugFile);
	CC.Ary2File(CC.reconstructAry, deCompressedFile, "Decompressed");


	//Close files
	inChainCodeFile.close();
	debugFile.close();
	deCompressedFile.close();
	printf("[Info] Closed all files\n");
}
