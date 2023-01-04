#pragma once

#include <fstream>

class BiMean
{
public:
	int numRows, numCols, minVal, maxVal, maxHeight, maxGVal, offSet, dividePt;
	int* histAry = nullptr, *GaussAry = nullptr;
	char** histGraph = nullptr, **GaussGraph = nullptr;

public:
	BiMean(int nRows, int nCols, int minVal, int maxVal, std::ifstream& inFile);
	int loadHist();
	void plotGraph(int* ary, char**& graph, const char symbol);
	void addVertical(char**& graph, const char symbol, const int bestThrVal);
	double computeMean(int leftIndex, int rightIndex, int maxHeight);
	double computeVar(int leftIndex, int rightIndex, int mean);
	double modifiedGauss(int x, int mean, int var, int maxHeight);
	void setZero(int* Ary);
	int biMeanGauss(int dividePt, std::ofstream& outFile2);
	double fitGauss(int leftIndex, int rightIndex, int* GaussAry);
	void bestFitGauss(int bestThrVal, int* GaussAry);
	char** plotAll();
};