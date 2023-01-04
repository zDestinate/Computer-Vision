#pragma once

#include <fstream>
#include <string>

class Morphology
{
public:
	int numImgRows, numImgCols, imgMin, imgMax, numStructRows, numStructCols, structMin, structMax, rowOrigin, colOrigin;
	int rowFrameSize, colFrameSize, extraRows, extraCols, rowSize, colSize;
	int** zeroFramedAry = nullptr;
	int** morphAry = nullptr;
	int** tempAry = nullptr;
	int** structAry = nullptr;

public:
	Morphology(int*, int*, int*);
	void zero2DAry(int**, int, int);
	void loadImg(std::ifstream&, int**);
	void loadstruct(std::ifstream&, int**);
	void ComputeDilation(int**, int**);
	void ComputeErosion(int**, int**);
	void ComputeOpening(int**, int**, int**);
	void ComputeClosing(int**, int**, int**);
	void onePixelDilation(int, int, int**, int**);
	void onePixelErosion(int, int, int**, int**);
	void AryToFile(int**, std::ofstream&);
	void prettyPrint(int**, std::ofstream&, int, int, int, int, std::string, bool=false);
};