#include "caculatePosition.h"
#include<iostream>
#include<cmath>
#include <iomanip>
#include <string.h>
#include <sstream>
#include <fstream>
#include <string>

void caculatePosition::caculateSatPos(std::vector<caculatePosition>& ca,double obsMoment)
{
	for (int num=0; num < ca.size(); num++)
	{
		//上述参数中，PRN为待匹配的PRN号，arr为输入的数据矩阵，包括导航文件的一些参数，obsMoment为观测时刻
		// 需要注意的是，观测时间也是gps周内秒
		//计算归化时间用的
		//计算规划时刻tk
		//计算卫星运动的平均角速度n
		double GM = 3.986005 * pow(10, 14);
		double n, n0;
		n0 = (sqrt(GM)) / pow(ca[num].satliteData[1][3], 3);
		n = n0 + ca[num].satliteData[0][2];

		//将toc时间转化为GPS周内秒
		caculatePosition cax = ca[num].ctimeTOgpst(ca[num]);

		//计算观测时刻卫星的平近角点
		double obs_deltaT = obsMoment - cax.sow;
		double deltat = ca[num].TOC[0] + ca[num].TOC[1] * obs_deltaT + TOC[2] * pow(obs_deltaT, 2);
		double t = obsMoment - deltat;
		double tk = t - ca[num].satliteData[2][0];//这个地方就是原理中的tk=t-toe
		double Mk = ca[num].satliteData[0][3] + n * tk;

		//计算偏近点角
		//此处采用迭代法求取偏近点角Ek
		const double pi = 3.14159265358979323846;
		Mk = std::remainder(Mk + 2 * pi, 2 * pi);
		double E = Mk;

		for (int i = 0; i < 10; i++) {
			int E_old = E;
			E = Mk + ca[num].satliteData[1][1] * std::sin(E);//ca.satliteData[1][1]为离心率
			double dE;
			dE = std::remainder(E - E_old, 2 * pi);
			if (std::abs(dE) < pow(10, -14))
			{
				break;
			}
		}
		//计算真近点角
		E = std::remainder(E + 2 * pi, 2 * pi);
		double e1 = 1 - pow(ca[num].satliteData[1][1], 2);
		double fs;
		fs = std::atan(sqrt(e1) * std::sin(E) / (std::cos(E) - ca[num].satliteData[1][1]));//嗯，根据公式算的

		//计算升交角距
		double u0 = ca[num].satliteData[3][2] + fs;
		u0 = std::remainder(u0, 2 * pi);

		//对升交角距，卫星矢径，卫星轨道倾角进行摄动改正
		//以下是摄动改正计算方法
		double sigema_u, sigema_r, sigema_i;
		double uk, rk, ik;
		sigema_u = ca[num].satliteData[1][0] * std::cos(2 * u0) + ca[num].satliteData[1][2] * std::sin(2 * u0);
		sigema_r = ca[num].satliteData[3][1] * std::cos(2 * u0) + ca[num].satliteData[0][1] * std::sin(2 * u0);
		sigema_i = ca[num].satliteData[2][1] * std::cos(2 * u0) + ca[num].satliteData[2][3] * std::sin(2 * u0);

		uk = u0 + sigema_u;
		rk = pow(ca[num].satliteData[1][3], 2) * (1 - ca[num].satliteData[1][1] * std::cos(E)) + sigema_r;
		ik = ca[num].satliteData[3][0] + sigema_i + ca[num].satliteData[4][0] * tk;

		//计算观测时刻的升交点经度L
		double Omegak;
		double we = 7.2921151467 * pow(10, -5);
		Omegak = ca[num].satliteData[2][2] + (ca[num].satliteData[3][3] - we) * tk - we * ca[num].satliteData[2][0];

		//卫星在轨道平面的坐标系中的位置
		double xk, yk;
		xk = rk * std::cos(uk);
		yk = rk * std::sin(uk);

		//计算卫星在WGS-84坐标系中的位置
		double X, Y, Z;
		X = xk * std::cos(Omegak) - yk * std::cos(ik) * std::sin(Omegak);
		Y = xk * std::sin(Omegak) + yk * std::cos(ik) * std::cos(Omegak);
		Z = yk * std::sin(ik);

		if (isnan(X)==true)
		{
			ca[num].count = -1;
		}
		else
		{
			ca[num].count = num;
		}
		ca[num].deltat = deltat;
		std::cout <<ca[num].PRN<< "卫星的坐标为" << X << " " << Y << " " << Z << std::endl;
		ca[num].x = -X;
		ca[num].y = -Y;
		ca[num].z = -Z;
		
	}
}

caculatePosition caculatePosition::ctimeTOgpst(caculatePosition ca)
{
	int year = ca.TOC_year+2000;
	int month = ca.TOC_month;
	int day = ca.TOC_day;
	int hour = ca.TOC_hour;
	int minute = ca.TOC_minute;
	double second = ca.TOC_second;
	int DayofMonth = 0;
	int DayofYear = 0;
	int weekno = 0;
	int dayofweek;
	int m;

	//计算从1980年到当前的前一年的天数,DOY,day of year
	for (m = 1980; m < year; m++)
	{
		if ((m % 4 == 0 && m % 100 != 0) || (m % 400 == 0))
		{
			DayofYear += 366;
		}
		else
			DayofYear += 365;
	}
	//计算当前一年内从元月到当前前一月的天数,DIY,day in year
	for (m = 1; m < month; m++)
	{
		if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
			DayofMonth += 31;
		else if (m == 4 || m == 6 || m == 9 || m == 11)
			DayofMonth += 30;
		else if (m == 2)
		{
			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
				DayofMonth += 29;
			else
				DayofMonth += 28;

		}
	}
	DayofMonth = DayofMonth + day - 6;//加上当月天数/减去1980年元月的6日

	ca.gpsweek= (DayofYear + DayofMonth) / 7;//计算GPS周,取整
	dayofweek = (DayofYear + DayofMonth) % 7;//取余,周内天
	//计算GPS时间
	ca.sow = dayofweek * 86400 + hour * 3600 + minute * 60 + second;
	return ca;
}

double caculatePosition::ctimeTOgpsweek(int a[6])
{

	int year = a[0] + 2000;
	int month = a[1];
	int day = a[2];
	int hour = a[3];
	int minute = a[4];
	double second = a[5];
	int DayofMonth = 0;
	int DayofYear = 0;
	int weekno = 0;
	int dayofweek;
	int m;

	//计算从1980年到当前的前一年的天数,DOY,day of year
	for (m = 1980; m < year; m++)
	{
		if ((m % 4 == 0 && m % 100 != 0) || (m % 400 == 0))
		{
			DayofYear += 366;
		}
		else
			DayofYear += 365;
	}
	//计算当前一年内从元月到当前前一月的天数,DIY,day in year
	for (m = 1; m < month; m++)
	{
		if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
			DayofMonth += 31;
		else if (m == 4 || m == 6 || m == 9 || m == 11)
			DayofMonth += 30;
		else if (m == 2)
		{
			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
				DayofMonth += 29;
			else
				DayofMonth += 28;

		}
	}
	DayofMonth = DayofMonth + day - 6;//加上当月天数/减去1980年元月的6日

	double gpsweek = (DayofYear + DayofMonth) / 7;//计算GPS周,取整
	dayofweek = (DayofYear + DayofMonth) % 7;//取余,周内天
	//计算GPS时间
	double sow = dayofweek * 86400 + hour * 3600 + minute * 60 + second;

	return gpsweek;
}

double caculatePosition::ctimeTOgpsSec(int a[6])
{

	int year = a[0] + 2000;
	int month = a[1];
	int day = a[2];
	int hour = a[3];
	int minute = a[4];
	double second = a[5];
	int DayofMonth = 0;
	int DayofYear = 0;
	int weekno = 0;
	int dayofweek;
	int m;

	//计算从1980年到当前的前一年的天数,DOY,day of year
	for (m = 1980; m < year; m++)
	{
		if ((m % 4 == 0 && m % 100 != 0) || (m % 400 == 0))
		{
			DayofYear += 366;
		}
		else
			DayofYear += 365;
	}
	//计算当前一年内从元月到当前前一月的天数,DIY,day in year
	for (m = 1; m < month; m++)
	{
		if (m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10 || m == 12)
			DayofMonth += 31;
		else if (m == 4 || m == 6 || m == 9 || m == 11)
			DayofMonth += 30;
		else if (m == 2)
		{
			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
				DayofMonth += 29;
			else
				DayofMonth += 28;

		}
	}
	DayofMonth = DayofMonth + day - 6;//加上当月天数/减去1980年元月的6日

	double gpsweek = (DayofYear + DayofMonth) / 7;//计算GPS周,取整
	dayofweek = (DayofYear + DayofMonth) % 7;//取余,周内天
	//计算GPS时间
	double sow = dayofweek * 86400 + hour * 3600 + minute * 60 + second;

	return sow;
}



