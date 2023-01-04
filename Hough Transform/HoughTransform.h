#pragma once

#include <fstream>
#include <string>

class HoughTransform
{
public:
	int numRows, numCols, minVal, maxVal, HoughDist, HoughAngle;
	int** imgAry = nullptr, **CartesianHoughAry = nullptr, **PolarHoughAry = nullptr;
	int angleInDegree = 0;
	double angleInRadians = 0;
	int offSet;

	HoughTransform(int*);
	void loadImage(std::ifstream&);
	void buildHoughSpace();
	void prettyPrint(int**, int, int, std::ofstream&, std::string);


//Private because it isn't use in main
private:
	void computeSinusoid(int, int);
	double CartesianDist(int, int, double);
	double PolarDist(int, int, double);
};