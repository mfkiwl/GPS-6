#pragma once
#include<vector>
#include<string>
#include <string.h>
#include <string>
class caculatePosition
{
public:
	caculatePosition() {
		
	}

	//这个表示一个特定卫星的PRN号码
	std::string PRN;
	int count;
	double deltat;//卫星钟差，平差的时候要用
	//这个表示卫星钟的数据time of clock，改正钟差的
	double TOC[3];
	//这个表示导航文件的主体数据，具体参考网上的数据解释就行，需要哪个用哪个
	double satliteData[6][4];
	//这个表示导航文件中最后一行的两个数，电文发送时刻与拟合区间
	double satliteDataElse[2];

	//TOC时刻？？？，咱也不知道这个是什么时间
	int TOC_year;
	int TOC_month;
	int TOC_day;
	int TOC_hour;
	int TOC_minute;
	double TOC_second;

	//GPS周与GPS周内秒
	long gpsweek;
	double sow;

	void caculateSatPos(std::vector<caculatePosition>& ca, double obsMoment);
	caculatePosition ctimeTOgpst(caculatePosition ca);
	double ctimeTOgpsweek(int a[6]);
	double ctimeTOgpsSec(int a[6]);

	double x;
	double y;
	double z;

};

//位置//笛卡尔坐标
struct postion
{
	double X, Y, Z;
};
//位置//大地坐标系
struct posblh
{
	double B, L, H;
};
//位置//站心坐标系
struct posenu
{
	double E, N, U;
};

