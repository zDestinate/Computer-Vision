#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>

using namespace std;

bool isDigit(string);
int* GetNumbers(string, char);

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


	string strLine;
	getline(inFile, strLine);
	int* pFirstLineData = GetNumbers(strLine, ' ');
	int numRows = pFirstLineData[0];
	int numCols = pFirstLineData[1];
	int minVal = pFirstLineData[2];
	int maxVal = pFirstLineData[3];
	delete[] pFirstLineData;


	string strUserInput;
	int thrValue;
	do
	{
		printf("Input threshold value: ");
		getline(cin, strUserInput);

		if (isDigit(strUserInput))
		{
			thrValue = stoi(strUserInput);
			if (thrValue >= minVal && thrValue <= maxVal)
			{
				break;
			}
			
			printf("[ERROR] Value is not within the min and max range (Min: %d, Max: %d)\n\n", minVal, maxVal);
		}
		else
		{
			printf("[ERROR] Invalid value. Value must be a digit/number\n\n");
		}
	} while (true);


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

	outFile1 << numRows << ' ' << numCols << " 0 1" << endl;
	outFile2 << numRows << ' ' << numCols << " 0 " << maxVal << endl;
	printf("[INFO] Successfully output the first line data to output files\n");


	while(getline(inFile, strLine))
	{
		int* pData = GetNumbers(strLine, ' ');
		for (int i = 0; i < numCols; i++)
		{
			if (pData[i] >= thrValue)
			{
				outFile1 << "1 ";
				outFile2 << pData[i] << ' ';
			}
			else
			{
				outFile1 << "0 ";
				outFile2 << "0  ";
			}
		}
		outFile1 << endl;
		outFile2 << endl;

		delete[] pData;
	}

	inFile.close();
	outFile1.close();
	outFile2.close();

	printf("[INFO] Completed!\n");
}

bool isDigit(string strInput)
{
	for (char const szChar : strInput)
	{
		if (!isdigit(szChar))
		{
			return false;
		}
	}

	return true;
}

int* GetNumbers(string strInput, char delimiter)
{
	int nDelimiter = 0;
	for (int i = 0; i < strInput.length(); i++)
	{
		if (strInput[i] == delimiter && (strInput[i + 1] != 0))
		{
			nDelimiter++;
		}
	}

	nDelimiter++;
	int* pNumbers = new int[nDelimiter];
	unsigned int nIndex = 0;

	string strTemp = "";
	for (int i = 0; i < strInput.length(); i++)
	{
		if (strInput[i] == delimiter)
		{
			if (isDigit(strTemp))
			{
				pNumbers[nIndex++] = stoi(strTemp);
			}
			strTemp = "";
			continue;
		}

		strTemp += strInput[i];
	}

	if (isDigit(strTemp) && (nIndex < nDelimiter))
	{
		pNumbers[nIndex] = stoi(strTemp);
	}

	return pNumbers;
}