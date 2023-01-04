#include <iostream>
#include <fstream>
#include <string>
#include "ExtractNum.h"
#include "BiMean.h"

using namespace std;

void OutputFile(ofstream&, string, char**, int);

int main(int argc, const char* argv[])
{
	if (argc != 4)
	{
		printf("[ERROR] Invalid arguments. Must have 3 arguments.\n");
		return 1;
	}

	ifstream inFile(argv[1]);
	if (!inFile)
	{
		printf("[ERROR] Invalid input file. Unable to open file!\n");
		return 1;
	}

	ofstream outFile1(argv[2]);
	if (!outFile1)
	{
		printf("[ERROR] Invalid out file 1\n");
		return 1;
	}

	ofstream outFile2(argv[3]);
	if (!outFile2)
	{
		printf("[ERROR] Invalid out file 2\n");
		return 1;
	}


	//Extract first line of the input file
	string strLine;
	getline(inFile, strLine);
	int* pFirstLineData = GetNumbers(strLine, '\t');
	int numRows = pFirstLineData[0];
	int numCols = pFirstLineData[1];
	int minVal = pFirstLineData[2];
	int maxVal = pFirstLineData[3];
	delete[] pFirstLineData;


	//BiMean Class
	printf("[BiMean] Start\n");
	BiMean BiMeanGauss(numRows, numCols, minVal, maxVal, inFile);
	BiMeanGauss.maxHeight = BiMeanGauss.loadHist();

	BiMeanGauss.plotGraph(BiMeanGauss.histAry, BiMeanGauss.histGraph, '*');
	OutputFile(outFile1, "histGraph plot", BiMeanGauss.histGraph, BiMeanGauss.maxHeight - 1);

	BiMeanGauss.offSet = (int)((BiMeanGauss.maxVal - BiMeanGauss.minVal) / 10);
	BiMeanGauss.dividePt = BiMeanGauss.offSet;

	int bestThrVal = BiMeanGauss.biMeanGauss(BiMeanGauss.dividePt, outFile2);
	BiMeanGauss.bestFitGauss(bestThrVal, BiMeanGauss.GaussAry);
	BiMeanGauss.plotGraph(BiMeanGauss.GaussAry, BiMeanGauss.GaussGraph, '+');
	OutputFile(outFile1, "GaussGraph plot", BiMeanGauss.GaussGraph, BiMeanGauss.maxHeight - 1);

	outFile1 << "Best threshold value: " << bestThrVal << endl << endl << endl;

	BiMeanGauss.addVertical(BiMeanGauss.histGraph, '|', bestThrVal);
	OutputFile(outFile1, "histGraph plot with vertical line", BiMeanGauss.histGraph, BiMeanGauss.maxHeight - 1);

	char** CombineGraphs = BiMeanGauss.plotAll();
	OutputFile(outFile1, "Plot all graphs", CombineGraphs, BiMeanGauss.maxHeight - 1);

	//Close files
	outFile1.close();
	outFile2.close();
}

bool ContainSymbol(char* szData)
{
	int nIndex = 0;
	while (szData[nIndex])
	{
		if (szData[nIndex] != ' ' && szData[nIndex] != '|')
		{
			return true;
		}
		nIndex++;
	}

	return false;
}

//This is for output the graph
void OutputFile(ofstream& outFile, string strCaption, char** szData, int len)
{
	outFile << strCaption << endl;
	for (int i = len; i > 0; i--)
	{
		if (ContainSymbol(szData[i]))
		{
			outFile << i + 1 << '\t' << szData[i] << endl;
		}
	}

	outFile << endl << endl;

	printf("[OutputFile] Successfully output %s\n", strCaption.c_str());
}