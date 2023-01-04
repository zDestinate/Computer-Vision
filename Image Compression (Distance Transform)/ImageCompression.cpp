#include "ImageCompression.h"
#include "ExtractNum.h"
#include <string>

using namespace std;

ImageCompression::ImageCompression(int* inData)
{
	printf("[ImageCompression] Class created\n");

	//First row data from imgFile
	numRows = inData[0];
	numCols = inData[1];
	minVal = inData[2];
	maxVal = inData[3];
	printf("[ImageCompression] inFile first line data loaded\n");


	ZFAry = new int* [numRows + 2];
	skeletonAry = new int* [numRows + 2];
	for (int i = 0; i < (numRows + 2); i++)
	{
		ZFAry[i] = new int[numCols + 2];
		skeletonAry[i] = new int[numCols + 2];
	}
	printf("[ImageCompression] Memory Allocated\n");
}

void ImageCompression::setZero(int** ary)
{
	for (int i = 0; i < (numRows + 2); i++)
	{
		for (int j = 0; j < (numCols + 2); j++)
		{
			ary[i][j] = 0;
		}
	}
}

void ImageCompression::loadImage(std::ifstream& inFile)
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
				ZFAry[i + 1][j + 1] = pData[j];
			}
		}
	}

	//Close file to save memory
	//No longer need since we loaded everything in the array
	inFile.close();

	printf("[ImageCompression] Loaded image\n");
}

void ImageCompression::loadSkeleton(std::ifstream& inFile, int** ary)
{
	string strLine;
	//Skip first line
	getline(inFile, strLine);

	//Loop until end of skeleton file
	while(getline(inFile, strLine))
	{
		int* pData = GetNumbers(strLine, ' ');
		ary[pData[0]][pData[1]] = pData[2];
	}

	inFile.close();

	printf("[ImageCompression] Loaded Skeleton\n");
}

void ImageCompression::compute8Distance(int** ary, ofstream& outFile)
{
	firstPass8Distance(ary);
	reformatPrettyPrint(ary, outFile, "First Pass Distance Transform");
	secondPass8Distance(ary);
	reformatPrettyPrint(ary, outFile, "Second Pass Distance Transform");
}

void ImageCompression::firstPass8Distance(int** ary)
{
	for (int i = 1; i <= numRows; i++)
	{
		for (int j = 1; j <= numCols; j++)
		{
			if (ary[i][j] > 0)
			{
				int a = ary[i - 1][j - 1];
				int b = ary[i - 1][j];
				int c = ary[i - 1][j + 1];
				int d = ary[i][j - 1];
				int min = GetMin(4, a + 1, b + 1, c + 1, d + 1);
				ary[i][j] = min;
			}
		}
	}
	printf("[ImageCompression] 1st pass distance transform compleleted\n");
}

void ImageCompression::secondPass8Distance(int** ary)
{
	for (int i = numRows; i > 0; i--)
	{
		for (int j = numCols; j > 0; j--)
		{
			if (ary[i][j] > 0)
			{
				int e = ary[i][j + 1];
				int f = ary[i + 1][j - 1];
				int g = ary[i + 1][j];
				int h = ary[i + 1][j + 1];
				int min = GetMin(5, e + 1, f + 1, g + 1, h + 1, ary[i][j]);
				ary[i][j] = min;
			}
		}
	}
	printf("[ImageCompression] 2nd pass distance transform compleleted\n");
}

void ImageCompression::imageCompression(int** ary, int** skeletonAry, std::ofstream& skeletonFile, std::ofstream& outFile)
{
	computeLocalMaxima(ary, skeletonAry);
	reformatPrettyPrint(skeletonAry, outFile, "Local Maxima");
	extractSkeleton(skeletonAry, skeletonFile);
}

bool ImageCompression::isLocalMaxima(int row, int col, int** ary)
{
	//Loop through all neighbor
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			//Skip self
			if (i == 0 && j == 0) continue;

			//Using negation
			//If any neighbor is > the current then its false
			if (ary[row + i][col + j] > ary[row][col]) return false;
		}
	}
	return true;
}

void ImageCompression::computeLocalMaxima(int** ary, int** ary2)
{
	for (int i = 1; i < (numRows + 1); i++)
	{
		for (int j = 1; j < (numCols + 1); j++)
		{
			if (isLocalMaxima(i, j, ary))
			{
				ary2[i][j] = ary[i][j];
			}
			else
			{
				ary2[i][j] = 0;
			}
		}
	}
	printf("[ImageCompression] Local maxima compleleted\n");
}

void ImageCompression::extractSkeleton(int** ary, std::ofstream& outFile)
{
	//Find the max for header file
	int max = 0;
	for (int i = 1; i <= numRows; i++)
	{
		for (int j = 1; j <= numCols; j++)
		{
			if (ary[i][j] > max)
			{
				max = ary[i][j];
			}
		}
	}

	//Header
	outFile << numRows << ' ' << numCols << ' ' << minVal << ' ' << max << endl;

	//Output the skelete image compression
	for (int i = 1; i <= numRows; i++)
	{
		for (int j = 1; j <= numCols; j++)
		{
			if (ary[i][j] > 0)
			{
				outFile << i << ' ' << j << ' ' << ary[i][j] << endl;
			}
		}
	}
	printf("[ImageCompression] Extracted skeleton lossless compression\n");
}

void ImageCompression::imageDeCompression(int** ary, std::ofstream& outFile)
{
	firstPassExpension(ary);
	reformatPrettyPrint(ary, outFile, "First Pass Expension");
	secondPassExpension(ary);
	reformatPrettyPrint(ary, outFile, "Second Pass Expension");
}

void ImageCompression::firstPassExpension(int** ary)
{
	for (int i = 1; i <= numRows; i++)
	{
		for (int j = 1; j <= numCols; j++)
		{
			if (ary[i][j] == 0)
			{
				int a = ary[i - 1][j - 1];
				int b = ary[i - 1][j];
				int c = ary[i - 1][j + 1];
				int d = ary[i][j - 1];
				int e = ary[i][j + 1];
				int f = ary[i + 1][j - 1];
				int g = ary[i + 1][j];
				int h = ary[i + 1][j + 1];
				int max = GetMax(9, a - 1, b - 1, c - 1, d - 1, e - 1, f - 1, g - 1, h - 1, ary[i][j]);
				ary[i][j] = max;
			}
		}
	}
	printf("[ImageCompression] 1st pass expension completed\n");
}

void ImageCompression::secondPassExpension(int** ary)
{
	for (int i = numRows; i > 0; i--)
	{
		for (int j = numCols; j > 0; j--)
		{
			int a = ary[i - 1][j - 1];
			int b = ary[i - 1][j];
			int c = ary[i - 1][j + 1];
			int d = ary[i][j - 1];
			int e = ary[i][j + 1];
			int f = ary[i + 1][j - 1];
			int g = ary[i + 1][j];
			int h = ary[i + 1][j + 1];
			int max = GetMax(9, a - 1, b - 1, c - 1, d - 1, e - 1, f - 1, g - 1, h - 1, ary[i][j]);
			if (ary[i][j] < max)
			{
				ary[i][j] = max;
			}
		}
	}
	printf("[ImageCompression] 2st pass expension completed\n");
}

void ImageCompression::threshold(int** ary, std::ofstream& outFile)
{
	outFile << numRows << ' ' << numCols << ' ' << minVal << ' ' << 1 << endl;

	for (int i = 1; i <= numRows; i++)
	{
		for (int j = 1; j <= numCols; j++)
		{
			if (ary[i][j] >= 1)
			{
				outFile << "1 ";
			}
			else
			{
				outFile << "0 ";
			}
		}
		outFile << endl;
	}
	printf("[ImageCompression] Threshold completed\n");
}

void ImageCompression::reformatPrettyPrint(int** ary, ofstream& outFile, string strCaption)
{
	//Caption
	outFile << strCaption << endl;

	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			if (ary[i + 1][j + 1] == 0)
			{
				outFile << ". ";
			}
			else
			{
				outFile << ary[i + 1][j + 1] << ' ';
			}
		}
		outFile << endl;
	}

	outFile << endl << endl << endl;
}




/* 
	=================================================

	Other functions that isn't part of the project
	They are use for computing

	=================================================
*/

//Get min number (Use for distance transform pass 1 & 2)
int ImageCompression::GetMin(unsigned int count, ...)
{
	int nMin = INT_MAX;
	va_list args;
	va_start(args, count);
	for (unsigned int i = 0; i < count; i++)
	{
		int num = va_arg(args, int);
		if (num < nMin) nMin = num;
	}

	va_end(args);
	return (nMin != INT_MAX) ? nMin : 0;
}

//Get max (Use for expansion 1 & 2)
int ImageCompression::GetMax(unsigned int count, ...)
{
	int nMax = INT_MIN;
	va_list args;
	va_start(args, count);
	for (unsigned int i = 0; i < count; i++)
	{
		int num = va_arg(args, int);
		if (num > nMax) nMax = num;
	}

	va_end(args);
	return (nMax != INT_MIN) ? nMax : 0;
}