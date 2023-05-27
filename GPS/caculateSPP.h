#pragma once
#include"caculatePosition.h"
#include"fileReader.h"
class caculatePPP
{
public:
	caculatePPP(){}

	void caculate(std::vector<caculatePosition>& ca, fileReader& fr,double X0,double Y0,double Z0);

private:
	double X;
	double Y;
	double Z;
	double DOP;
};

