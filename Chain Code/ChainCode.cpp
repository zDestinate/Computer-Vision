#include "ChainCode.h"
#include "ExtractNum.h"

using namespace std;

ChainCode::ChainCode(int* pFirstLineData)
{
	printf("[ChainCode] Class created\n");

	//Load header
	numRows = pFirstLineData[0];
	numCols = pFirstLineData[1];
	minVal = pFirstLineData[2];
	maxVal = pFirstLineData[3];
	printf("[ChainCode] inFile first line data loaded\n");


	//Memory allocate
	imgAry = new int* [numRows + 2];
	reconstructAry = new int* [numRows + 2];
	for (int i = 0; i < numRows + 2; i++)
	{
		imgAry[i] = new int[numCols + 2];
		reconstructAry[i] = new int[numCols + 2];
		for (int j = 0; j < numCols + 2; j++)
		{
			imgAry[i][j] = 0;
			reconstructAry[i][j] = 0;
		}
	}

	coordOffset[0].row = 0; coordOffset[0].col = 1;
	coordOffset[1].row = -1; coordOffset[1].col = 1;
	coordOffset[2].row = -1; coordOffset[2].col = 0;
	coordOffset[3].row = -1; coordOffset[3].col = -1;
	coordOffset[4].row = 0; coordOffset[4].col = -1;
	coordOffset[5].row = 1; coordOffset[5].col = -1;
	coordOffset[6].row = 1; coordOffset[6].col = 0;
	coordOffset[7].row = 1; coordOffset[7].col = 1;

	printf("[ChainCode] Memory Allocated\n");
}

void ChainCode::loadImage(std::ifstream& inFile)
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
				imgAry[i + 1][j + 1] = pData[j];
			}
		}
	}

	//Close file to save memory
	//No longer need since we loaded everything in the array
	inFile.close();

	printf("[ChainCode] Loaded image\n");
}

void ChainCode::getChainCode(int** imgAry, std::ofstream& chainCodeFile)
{
	chainCodeFile << numRows << ' ' << numCols << ' ' << minVal << ' ' << maxVal << endl;
	int startRow, startCol, lastQ;

	for (int i = 1, found = 0; i < numRows + 1 && !found; i++)
	{
		for (int j = 1; j < numCols + 1 && !found; j++)
		{
			if (imgAry[i][j] > 0)
			{
				startRow = i;
				startCol = j;
				chainCodeFile << imgAry[i][j] << ' ' << i << ' ' << j << endl;
				startP.row = i;
				startP.col = j;
				currentP.row = i;
				currentP.col = j;
				lastQ = 4;
				
				//We found the starting point so we will exist the nested loop
				found = 1;
			}
		}
	}

	do
	{
		lastQ = (lastQ + 1) % 8;
		chainDir = findNextP(currentP, lastQ);
		chainCodeFile << chainDir << ' ';
		nextP.row = currentP.row + coordOffset[chainDir].row;
		nextP.col = currentP.col + coordOffset[chainDir].col;
		currentP = nextP;
		//In case chainDir = 0 then (chainDir - 1) % 8 = -1
		//Which it should be 7 so i changed this part
		lastQ = zeroTable[(8 + ((chainDir - 1) % 8)) % 8];
	} while (currentP.row != startRow || currentP.col != startCol);

	printf("[ChainCode] Get chaincode completed\n");
}

int ChainCode::findNextP(point currentP, int lastQ)
{
	int index = lastQ;
	bool found = false;
	while (!found)
	{
		int iRow = currentP.row + coordOffset[index].row;
		int jCol = currentP.col + coordOffset[index].col;
		if (imgAry[iRow][jCol] == label)
		{
			chainDir = index;
			found = true;
		}
		else
		{
			index = (index + 1) % 8;
		}
	}

	return chainDir;
}

void ChainCode::deCompression(int** reconstructAry, ifstream& chainCodeFile, ofstream& debugFile)
{
	string strLine;
	//Reading header but won't use them
	getline(chainCodeFile, strLine);
	int* pFirstLineData = GetNumbers(strLine, ' ');

	//Grab label row col
	getline(chainCodeFile, strLine);
	int* pData = GetNumbers(strLine, ' ');
	int label = pData[0];
	int startRow = pData[1];
	int startCol = pData[2];
	delete[] pData;

	reconstructAry[startRow][startCol] = label;
	int mark = -1 * label;
	point P;
	P.row = startRow;
	P.col = startCol;
	lastZero = 4;

	//Get chaincode data
	getline(chainCodeFile, strLine);
	int nChainCodeLength;
	int* pChainCodeData = GetNumbers(strLine, ' ', nChainCodeLength);
	
	//Loop thru all the chaincode
	for (int i = 0; i < nChainCodeLength; i++)
	{
		int nextChainCode = pChainCodeData[i];
		while (lastZero != nextChainCode)
		{
			int r = P.row + coordOffset[lastZero].row;
			int c = P.col + coordOffset[lastZero].col;
			if (reconstructAry[r][c] == 0)
			{
				reconstructAry[r][c] = mark;
			}
			else
			{
				debugFile << "[Decompression] Something is wrong " << r << ' ' << c << endl;
			}

			lastZero++;
			lastZero = lastZero % 8;
		}

		P.row = P.row + coordOffset[nextChainCode].row;
		P.col = P.col + coordOffset[nextChainCode].col;
		lastZero = zeroTable[(8 + ((nextChainCode - 1) % 8)) % 8];

		//Add label to make it easier to fill them later
		reconstructAry[P.row][P.col] = label;


		if ((P.row == startRow) && (P.col == startCol) && (i != 0))
		{
			break;
		}
	}

	Ary2File(reconstructAry, debugFile, "Decompression (Reconstruct)");
	fillInterior(reconstructAry, label);
	Ary2File(reconstructAry, debugFile, "Decompression (Filled Interior)");
	threshold(reconstructAry);

	printf("[ChainCode] Decompress completed\n");
}

void ChainCode::fillInterior(int** reconstructAry, int label)
{
	int c = 1;
	for (int i = 1; i < numRows + 1; i++)
	{
		do
		{
			//Find the beginning column and ending column
			int* pPoint = FindAdjacent(reconstructAry, i, c, label);

			//If they aren't -1 then we will fill those columns
			if ((pPoint[0] != -1) && (pPoint[1] != -1))
			{
				for (int j = pPoint[0]; j < pPoint[1]; j++)
				{
					reconstructAry[i][j] = 1;
				}
			}

			//Set the next column the last point
			c = pPoint[1];
		} while (c != -1 && c < (numCols + 1));
		c = 1;
	}

	printf("[ChainCode] Fill Interior completed\n");
}

void ChainCode::threshold(int** reconstructAry)
{
	int MaxRows = numRows + 2, MaxCols = numCols + 2;
	for (int i = 0; i < MaxRows; i++)
	{
		for (int j = 0; j < MaxCols; j++)
		{
			if (reconstructAry[i][j] < 0)
			{
				reconstructAry[i][j] = 0;
			}
		}
	}

	printf("[ChainCode] Threshold completed\n");
}

void ChainCode::Ary2File(int** ary, ofstream& outFile, string strCaption)
{
	reformatPrettyPrint(ary, outFile, strCaption);
}

void ChainCode::reformatPrettyPrint(int** ary, ofstream& outFile, string strCaption)
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
			else if (ary[i + 1][j + 1] < 0)
			{
				outFile << "- ";
			}
			else
			{
				outFile << ary[i+1][j+1] << ' ';
			}
		}
		outFile << endl;
	}

	outFile << endl << endl << endl;
}


/*
	PRIVATE FUNCTIONS
*/
int* ChainCode::FindAdjacent(int** reconstructAry, int r, int c, int label)
{
	int nRet[2] = {-1, -1};
	bool bFound = false;
	for (int i = c; i < numCols + 1; i++)
	{
		if (!bFound && reconstructAry[r][i] == 1)
		{
			nRet[0] = i;
			bFound = true;
		}
		else if (bFound&& reconstructAry[r][i] < 0 && reconstructAry[r][i - 1] == 1)
		{
			nRet[1] = i;
			break;
		}
	}

	return nRet;
}