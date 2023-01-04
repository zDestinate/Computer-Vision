#pragma once

#include <fstream>
#include <string>
#include <cstdarg>

class ImageCompression
{
public:
	int numRows, numCols, minVal, maxVal;
	int newMinVal = 0, newMaxVal = 0;
	int** ZFAry = nullptr;
	int** skeletonAry = nullptr;

	ImageCompression(int*);
	void setZero(int**);
	void loadImage(std::ifstream&);
	void loadSkeleton(std::ifstream&, int**);
	void compute8Distance(int**, std::ofstream&);
	void firstPass8Distance(int**);
	void secondPass8Distance(int**);
	void imageCompression(int**, int**, std::ofstream&, std::ofstream&);
	bool isLocalMaxima(int, int, int**);
	void computeLocalMaxima(int**, int**);
	void extractSkeleton(int**, std::ofstream&);
	void imageDeCompression(int**, std::ofstream&);
	void firstPassExpension(int**);
	void secondPassExpension(int**);
	void threshold(int**, std::ofstream&);
	void reformatPrettyPrint(int**, std::ofstream&, std::string);

//Other functions that isn't part of the project which use for computing
private:
	int GetMin(unsigned int, ...);
	int GetMax(unsigned int, ...);
};