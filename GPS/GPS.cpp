#include"fileReader.h"
#include"caculatePosition.h"
#include"caculateSPP.h"
#include<iostream>
#include<vector>
#include<fstream>
using namespace std;
int main()
{
	fileReader fr;
	
	double x = 0, y = 0, z = 0;

	struct obs_head ohead;
	struct obs_epoch oepoch;
	string broadcastFile = "D:/code/c++ code/GPS/file/abpo3080.22o";
	//string navigationFile = "D:/OneDrive - cumt.edu.cn/桌面/卫星导航定位程序设计/308的卫星星历与观测值文件/file/abpo3080.22n";
	string navigationFile = "D:/code/c++ code/GPS/file/BRDC_308.rnx";
	//我为什么要分开写呢？这里就体现了重载的构造函数的作用
	fileReader fr1(broadcastFile, navigationFile);

	//这个readFile_O函数运行完了之后主要观测数据都保存在fr1对象之中
	//头部数据保存在ohead结构体对象中，观测历元数据保存在oepoch中
	fr.readFile_O(ohead, oepoch,broadcastFile,fr1); 
	oepoch.y = oepoch.y + 2000;

	//o文件读取完毕整理一下数据
	double X0 = ohead.apX;//粗略坐标
	double Y0 = ohead.apY;//粗略坐标
	double Z0 = ohead.apZ;//粗略坐标

	//卫星数目与类型
	int sateliteNum = oepoch.sat_num;
	string sateliteName = oepoch.satName;

	//观测时间此时存储在oepoch对象中
	//观测的伪距，载波相位观测值在fr1.observation中


	//接下来将卫星顺序用一个字符串数组与一个整型数组存储起来，存到numPRN
	std::vector<int> vecNum;
	fr.getPRN(oepoch, oepoch.sat_num, vecNum);

	//获取对应的卫星名称保存到SatliteType字符数组中
	std::vector<char> vecChar;
	fr.getSatliteType(oepoch, oepoch.sat_num, vecChar);

	std::vector<char> vecString;
	for (int i = 0; i < oepoch.sat_num ; i++) {
		std::string substr = oepoch.satName.substr(3 * i, 3);
		for (char c : substr) {
			vecString.push_back(c);
		}
	}
	//读取前N个卫星数据
	int N = 400;
	std::vector<navi_body> nbody(N);
	std::vector<navi_body> nbody1(oepoch.sat_num);

	fr.readFile_N(nbody, navigationFile, fr1, 96);

	for (int i = 0; i < oepoch.sat_num; i++) {
		std::string substr = oepoch.satName.substr(3 * i, 3);
		for (int j = 0; j < N; j++) {
			if (nbody[j].PRN == substr)
			{
				if (nbody[j].TOC_year == oepoch.y && nbody[j].TOC_month == oepoch.m && nbody[j].TOC_day == oepoch.d)
				{
					if (nbody[j].TOC_hour == oepoch.h && nbody[j].TOC_minute == oepoch.min && nbody[j].TOC_second == oepoch.sec)
					{
						fr.nbodyTonbody(nbody1[i], nbody[j]);
					}
				}
			}
		}
	}
	//卫星位置计算
	//将读取的导航文件数据传入到计算坐标类体的对象
	std::vector<caculatePosition> ca(oepoch.sat_num);
	std::vector<caculatePosition>  ca1=fr.nbodyTocacupos(nbody1, ca);

	//实例化一个函数调用对象
	caculatePosition caDioaYong;
	//测试计算一个卫星的坐标
		int b[6] = { oepoch.y-2000,oepoch.m,oepoch.d,oepoch.h,oepoch.min,oepoch.sec};
		double tem = caDioaYong.ctimeTOgpsSec(b);
		caDioaYong.caculateSatPos(ca1, tem);

	//实例化一个单点定位计算对象
		caculatePPP cap;

		cap.caculate(ca1, fr1, X0, Y0, Z0);

}

