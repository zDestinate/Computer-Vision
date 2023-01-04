#include <iostream>
#include <string>
#include <math.h>
#include "ExtractNum.h"
#include "BiMean.h"

using namespace std;

BiMean::BiMean(int nRows, int nCols, int minVal, int maxVal, ifstream &inFile)
{
	this->numRows = nRows;
	this->numCols = nCols;
	this->minVal = minVal;
	this->maxVal = maxVal;

	GaussAry = new int[maxVal + 1];
	histAry = new int[maxVal + 1];

	offSet = (int)(maxVal - minVal) / 10;
	dividePt = offSet;

	//Load input file and store it in histAry
	string strLine;

	//Starting from next line until it empty
	int nCounter = 0;
	while (getline(inFile, strLine) && nCounter <= maxVal)
	{
		int* pData = GetNumbers(strLine, '\t');
		histAry[pData[0]] = pData[1];
		delete[] pData;
		nCounter++;
	}

	inFile.close();

	printf("[BiMean] Constructor created\n");
}

int BiMean::loadHist()
{
	int nIndex = 0;
	int nMaxValue = 0;

	for (int i = 0; i <= maxVal; i++)
	{
		if (histAry[i] > nMaxValue)
		{
			nIndex = i;
			nMaxValue = histAry[i];
		}
	}

	printf("[BiMean] loadHist completed\n");
	return nMaxValue;
}

void BiMean::plotGraph(int* ary, char**& graph, const char symbol)
{
	maxHeight = 0;
	for (int i = 0; i <= maxVal; i++)
	{
		if (ary[i] > maxHeight)
		{
			maxHeight = ary[i];
		}
	}
	graph = new char*[maxHeight + 1];

	for (int i = 0; i < maxHeight; i++)
	{
		string strTemp = "";
		for (int j = 0; j <= maxVal; j++)
		{
			strTemp += ' ';
			if (ary[j] == i + 1)
			{
				strTemp += symbol;
			}
			else
			{
				strTemp += ' ';
			}
		}

		graph[i] = new char[strTemp.length() + 1];
		memcpy(graph[i], strTemp.c_str(), strTemp.length());

		//null terminator
		graph[i][strTemp.length()] = 0;
	}

	printf("[BiMean] plotGraph Completed\n");
}

void BiMean::addVertical(char**& graph, const char symbol, const int bestThrVal)
{
	for (int i = 0; i < maxHeight; i++)
	{
		graph[i][(2 * bestThrVal) + 1] = symbol;
	}
}

double BiMean::computeMean(int leftIndex, int rightIndex, int maxHeight)
{
	this->maxHeight = 0;
	int sum = 0;
	int numPixels = 0;
	int index = leftIndex;

	while (index < rightIndex)
	{
		sum += histAry[index] * index;
		numPixels += histAry[index];
		if (histAry[index] > this->maxHeight)
		{
			this->maxHeight = histAry[index];
		}
		index++;
	}

	return (double)(sum / numPixels);
}

double BiMean::computeVar(int leftIndex, int rightIndex, int mean)
{
	double sum = 0.0;
	int numPixels = 0;
	int index = leftIndex;

	while (index < rightIndex)
	{
		sum += (double)(histAry[index] * pow(index - mean, 2));
		numPixels += histAry[index];
		index++;
	}

	return (double)(sum / numPixels);
}

double BiMean::modifiedGauss(int x, int mean, int var, int maxHeight)
{
	return (double)(maxHeight * exp(-(pow(x - mean, 2) / (2 * var))));
}

void BiMean::setZero(int* Ary)
{
	for (int i = 0; i < maxVal; i++)
	{
		Ary[i] = 0;
	}
}

int BiMean::biMeanGauss(int dividePt, ofstream& outFile2)
{
	double sum1, sum2, total, minSumDiff;
	int bestThr = dividePt;
	minSumDiff = 999999.0;

	while (dividePt < (maxVal - offSet))
	{
		setZero(GaussAry);
		sum1 = fitGauss(0, dividePt, GaussAry);
		sum2 = fitGauss(dividePt, maxVal, GaussAry);
		total = sum1 + sum2;
		if (total < minSumDiff)
		{
			minSumDiff = total;
			bestThr = dividePt;
		}

		outFile2 << dividePt << ", " << sum1 << ", " << sum2 << ", " << total << ", " << minSumDiff << ", " << bestThr << endl;
		dividePt++;
	}
	
	printf("[BiMean] biMeanGauss Completed (bestThr: %d)\n", bestThr);
	return bestThr;
}

double BiMean::fitGauss(int leftIndex, int rightIndex, int* GaussAry)
{
	double mean, var, sum, Gval, maxGval;
	sum = 0.0;

	mean = computeMean(leftIndex, rightIndex, maxHeight);
	var = computeVar(leftIndex, rightIndex, mean);

	int index = leftIndex;

	while (index <= rightIndex)
	{
		Gval = modifiedGauss(index, mean, var, maxHeight);
		sum += abs(Gval - (double)histAry[index]);
		GaussAry[index] = (int)Gval;
		index++;
	}
	
	return sum;
}

void BiMean::bestFitGauss(int bestThrVal, int* GaussAry)
{
	double sum1, sum2;
	setZero(GaussAry);
	sum1 = fitGauss(0, bestThrVal, GaussAry);
	sum2 = fitGauss(bestThrVal, maxVal, GaussAry);

	printf("[BiMean] bestFitGauss Completed\n");
}

char** BiMean::plotAll()
{
	char** ret = histGraph;
	for (int i = 0; i < maxHeight; i++)
	{
		for (int j = 0; j <= maxVal * 2; j++)
		{
			if (ret[i][j] == '|')
			{
				continue;
			}

			if (ret[i][j] != GaussGraph[i][j] && GaussGraph[i][j] != ' ')
			{
				ret[i][j] = GaussGraph[i][j];
			}
		}
	}

	return ret;
}