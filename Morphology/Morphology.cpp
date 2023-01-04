#include "Morphology.h"
#include "ExtractNum.h"
#include <string>

using namespace std;

Morphology::Morphology(int* ImgData, int* StructData, int* OriginData)
{
	printf("[Morphology] Class created\n");

	//First row data from imgFile
	numImgRows = ImgData[0];
	numImgCols = ImgData[1];
	imgMin = ImgData[2];
	imgMax = ImgData[3];
	printf("[Morphology] imgFile first line data loaded\n");

	//First row data from structFile
	numStructRows = StructData[0];
	numStructCols = StructData[1];
	structMin = StructData[2];
	structMax = StructData[3];
	printf("[Morphology] structFile first line data loaded\n");

	//Origin data from structFile
	rowOrigin = OriginData[0];
	colOrigin = OriginData[1];
	printf("[Morphology] structFile origin data loaded\n");


	//Set initialized value
	rowFrameSize = numStructRows / 2;
	colFrameSize = numStructCols / 2;
	extraRows = rowFrameSize * 2;
	extraCols = colFrameSize * 2;
	rowSize = numImgRows + extraRows;
	colSize = numImgCols + extraCols;


	//Allocate memory
	zeroFramedAry = new int*[rowSize];
	morphAry = new int* [rowSize];
	tempAry = new int* [rowSize];
	for (int i = 0; i < rowSize; i++)
	{
		zeroFramedAry[i] = new int[colSize];
		morphAry[i] = new int[colSize];
		tempAry[i] = new int[colSize];
	}

	structAry = new int* [numStructRows];
	for (int i = 0; i < numStructRows; i++)
	{
		structAry[i] = new int[numStructCols];
	}
	printf("[Morphology] Memory Allocated\n");
}

void Morphology::zero2DAry(int** Ary, int nRow, int nCol)
{
	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			Ary[i][j] = 0;
		}
	}
}

void Morphology::loadImg(std::ifstream& inFile, int** Ary)
{
	string strLine;
	for (int i = 0; i < numImgRows; i++)
	{
		getline(inFile, strLine);
		if (strLine.length())
		{
			int* pData = GetNumbers(strLine, ' ');
			for (int j = 0; j < numImgCols; j++)
			{
				Ary[i + rowOrigin][j + colOrigin] = pData[j];
			}
		}
	}

	//Close file to save memory
	//No longer need since we loaded everything in the array
	inFile.close();

	printf("[Morphology] Loaded image\n");
}

void Morphology::loadstruct(std::ifstream& inFile, int** Ary)
{
	string strLine;
	for (int i = 0; i < numStructRows; i++)
	{
		getline(inFile, strLine);
		if (strLine.length())
		{
			int* pData = GetNumbers(strLine, ' ');
			for (int j = 0; j < numStructCols; j++)
			{
				Ary[i][j] = pData[j];
			}
		}
	}

	//Close file to save memory
	//No longer need since we loaded everything in the array
	inFile.close();

	printf("[Morphology] Loaded struct\n");
}

void Morphology::ComputeDilation(int** inAry, int** outAry)
{
	for (int i = rowFrameSize; i < rowSize; i++)
	{
		for (int j = colFrameSize; j < colSize; j++)
		{
			if (inAry[i][j] > 0)
			{
				onePixelDilation(i, j, inAry, outAry);
			}
		}
	}
}

void Morphology::ComputeErosion(int** inAry, int** outAry)
{
	for (int i = rowFrameSize; i < rowSize; i++)
	{
		for (int j = colFrameSize; j < colSize; j++)
		{
			if (inAry[i][j] > 0)
			{
				onePixelErosion(i, j, inAry, outAry);
			}
		}
	}
}

void Morphology::ComputeOpening(int** Ary, int** computeAry, int** tmpAry)
{
	ComputeErosion(Ary, tmpAry);
	ComputeDilation(tmpAry, computeAry);
}

void Morphology::ComputeClosing(int** Ary, int** computeAry, int** tmpAry)
{
	ComputeDilation(Ary, tmpAry);
	ComputeErosion(tmpAry, computeAry);
}

void Morphology::onePixelDilation(int i, int j, int** inAry, int** outAry)
{
	int iOffset = i - rowOrigin;
	int jOffset = j - colOrigin;
	for (int rIndex = 0; rIndex < numStructRows; rIndex++)
	{
		for (int cIndex = 0; cIndex < numStructCols; cIndex++)
		{
			if (structAry[rIndex][cIndex] > 0)
			{
				outAry[iOffset + rIndex][jOffset + cIndex] = 1;
			}
		}
	}
}

void Morphology::onePixelErosion(int i, int j, int** inAry, int** outAry)
{
	int iOffset = i - rowOrigin;
	int jOffset = j - colOrigin;
	bool matchFlag = true;

	for (int rIndex = 0; rIndex < numStructRows && matchFlag; rIndex++)
	{
		for (int cIndex = 0; cIndex < numStructCols && matchFlag; cIndex++)
		{
			if ((structAry[rIndex][cIndex] > 0) && (inAry[iOffset + rIndex][jOffset + cIndex] <= 0))
			{
				matchFlag = false;
			}
		}
	}

	if (matchFlag)
	{
		outAry[i][j] = 1;
	}
	else
	{
		outAry[i][j] = 0;
	}
}

void Morphology::AryToFile(int** Ary, std::ofstream& outFile)
{
	//Header
	string strFirstLine = to_string(numImgRows) + ' ' + to_string(numImgCols) + ' ' + to_string(imgMin) + ' ' + to_string(imgMax);
	outFile << strFirstLine << endl;

	for (int i = rowFrameSize; i < rowSize - rowFrameSize; i++)
	{
		for (int j = colFrameSize; j < colSize - colFrameSize; j++)
		{
			outFile << Ary[i][j] << ' ';
		}
		outFile << endl;
	}
}

void Morphology::prettyPrint(int** Ary, std::ofstream& outFile, int nRow, int nCol, int nMin, int nMax, string strCaption, bool bStruct)
{
	//Caption
	outFile << strCaption << endl;

	//Header
	string strFirstLine = to_string(nRow) + ' ' + to_string(nCol) + ' ' + to_string(nMin) + ' ' + to_string(nMax);
	outFile << strFirstLine << endl;


	//Check if it struct then we will add the origin data to header
	if (bStruct)
	{
		outFile << to_string(rowOrigin) << ' ' << to_string(colOrigin) << endl;
	}

	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			if (Ary[i][j] == 0)
			{
				outFile << ". ";
			}
			else
			{
				outFile << "1 ";
			}
		}
		outFile << endl;
	}

	outFile << endl << endl << endl;
}
