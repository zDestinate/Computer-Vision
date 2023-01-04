#include <ctype.h>
#include "ExtractNum.h"

using namespace std;

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
		if (strInput[i] == delimiter && (strTemp.length()))
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