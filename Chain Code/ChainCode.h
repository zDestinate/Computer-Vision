#pragma once

#include <fstream>
#include <string>

class point
{
public:
	int row, col;
};

class ChainCode
{
public:
	int numRows, numCols, minVal, maxVal, label = 1;
	int** imgAry = nullptr;
	int** reconstructAry = nullptr;
	point coordOffset[8];
	int zeroTable[8] = { 6, 0, 0, 2, 2, 4, 4, 6 };
	point startP, currentP, nextP;
	int lastZero, chainDir;

	ChainCode(int*);
	void loadImage(std::ifstream&);
	void getChainCode(int**, std::ofstream&);
	int findNextP(point, int);
	void deCompression(int**, std::ifstream&, std::ofstream&);
	void fillInterior(int**, int);
	void threshold(int**);
	void Ary2File(int**, std::ofstream&, std::string);
	void reformatPrettyPrint(int**, std::ofstream&, std::string);

private:
	int* FindAdjacent(int**, int, int, int);
};
