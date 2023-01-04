
//This is for math constant like M_PI for pi value
#define _USE_MATH_DEFINES

#include "HoughTransform.h"
#include "ExtractNum.h"
#include <math.h>

using namespace std;

HoughTransform::HoughTransform(int* pFirstLineData)
{
	printf("[HoughTransform] Class created\n");

	//Load header
	numRows = pFirstLineData[0];
	numCols = pFirstLineData[1];
	minVal = pFirstLineData[2];
	maxVal = pFirstLineData[3];
	printf("[HoughTransform] inFile first line data loaded\n");


	//Memory allocate
	offSet = sqrt(pow(numRows, 2) + pow(numCols, 2));
	HoughAngle = 180;
	HoughDist = 2 * offSet;

	imgAry = new int*[numRows];
	for (int i = 0; i < numRows; i++)
	{
		imgAry[i] = new int[numCols];
	}
	
	CartesianHoughAry = new int* [HoughDist];
	PolarHoughAry = new int* [HoughDist];
	for (int i = 0; i < HoughDist; i++)
	{
		CartesianHoughAry[i] = new int[HoughAngle];
		PolarHoughAry[i] = new int[HoughAngle];
		for (int j = 0; j < HoughAngle; j++)
		{
			CartesianHoughAry[i][j] = 0;
			PolarHoughAry[i][j] = 0;
		}
	}

	printf("[HoughTransform] Memory Allocated\n");
}

void HoughTransform::loadImage(std::ifstream& inFile)
{
	string strLine;

	for (int i = 0; i < numRows; i++)
	{
		getline(inFile, strLine);
		if (strLine.length())
		{
			int* pData = GetNumbers(strLine, ' ');
			for (int j = 0; j < numCols; j++)
			{
				imgAry[i][j] = pData[j];
			}
		}
	}

	//Close file to save memory
	//No longer need since we loaded everything in the array
	inFile.close();

	printf("[HoughTransform] Loaded image\n");
}

void HoughTransform::buildHoughSpace()
{
	for (int x = 0; x < numRows; x++)
	{
		for (int y = 0; y < numCols; y++)
		{
			if (imgAry[x][y] > 0)
			{
				computeSinusoid(x, y);
			}
		}
	}
	printf("[HoughTransform] Hough space completed\n");
}

void HoughTransform::computeSinusoid(int x, int y)
{
	angleInDegree = 0;
	do
	{
		angleInRadians = angleInDegree / 180.00 * M_PI;

		double dist = CartesianDist(x, y, angleInRadians);
		int distInt = (int)dist;
		CartesianHoughAry[distInt][angleInDegree]++;

		dist = PolarDist(x, y, angleInRadians);
		distInt = (int)dist;
		PolarHoughAry[distInt][angleInDegree]++;
		angleInDegree++;
	} while (angleInDegree <= 179);
}

double HoughTransform::CartesianDist(int x, int y, double radian)
{
	double t = radian - atan2(y, x) - (M_PI / 2);
	double d = sqrt(pow(x, 2) + pow(y, 2)) * cos(t) + offSet;
	return d;
}

double HoughTransform::PolarDist(int x, int y, double radian)
{
	double p = ((double)x * cos(radian)) + ((double)y * sin(radian)) + offSet;
	return p;
}

void HoughTransform::prettyPrint(int** ary, int row, int col, ofstream& outFile, string strCaption)
{
	//Caption
	outFile << strCaption << endl;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (ary[i][j] == 0)
			{
				outFile << ". ";
			}
			else
			{
				outFile << ary[i][j] << ' ';
			}
		}
		outFile << endl;
	}

	outFile << endl << endl << endl;
}