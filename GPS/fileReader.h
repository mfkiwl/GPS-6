#pragma once
#include <string>
#include <string.h>
#include"caculatePosition.h"
#include<vector>
using namespace std;
class fileReader
{
public:
	//构造函数的重载
	fileReader() {

	}
	fileReader(string filepath,string filepath_n) 
	{
		this->filepath = filepath;
		this->filepath_n = filepath_n;
	}
	/*定义读取和写的O文件与N文件*/
	void readFile_O(struct obs_head& ohead, struct obs_epoch& oepoch, string broadcastFile, fileReader& filereader);
	void readFile_N(std::vector<struct navi_body>& nbody, string navigationFile, fileReader& filereader, int allHangNum_n);
	int getHeaderNum(string navigationFile);
	string getLine(const std::string& fileName, int lineNumber);

	//将结构体navi_body的数据传入caculatePosition类体中
	std::vector<caculatePosition> nbodyTocacupos(std::vector<struct navi_body>& nbody, std::vector<caculatePosition>& ca);

	void nbodyTonbody(struct navi_body& nbody1, struct navi_body& nbody2);

	//获取卫星的PRN号,其中，第二个为卫星的数量
	void getPRN(struct obs_epoch& oepoch, int num, std::vector<int>& vec);

	//获取伪距,获取任意一行的伪距
	double getobsL1(fileReader fr,int col);

	void setobsL1(fileReader& fr, int col, double N);

	//获取卫星的类型,，第二个为卫星的数量
	void getSatliteType(struct obs_epoch& oepoch, int num, std::vector<char>& vec);

	//获取初始化坐标（粗略坐标）
	int getInitPosition(string fileName);

	//读取文件时去除文件中的空格,如果为空赋值“10000000”
	string trim(string& s);
	//这两个函数主要是为了实现读取特定行
	//具体我也不知道怎么实现的，网上抄的
	int CountLines(string filename);
	string ReadLine(string filename, int line);
private:
	string filepath;
	string filepath_n;
	//先用一个历元的数据试试
	double observation[25][8];
};
//观测值头文件的结构体
typedef struct obs_head
{
	double ver;//RINEX文件版本号
	string type;//文件类型
	double apX;//测站近似位置XYZ（WGS84）
	double apY;
	double apZ;
	int obstypenum;//观测值类型数量
	string obstype;//rinex观测值类型列表
	double interval;//观测值的历元间隔
	int f_y;//第一个观测记录的时刻
	int f_m;
	int f_d;
	int f_h;
	int f_min;
	double f_sec;
	char tsys[5];//时间系统
};


//观测值历元数据结构体
typedef struct obs_epoch
{
	//观测历元时刻
	int y;
	int m;
	int d;
	int h;
	int min;
	double sec;
	int p_flag;//历元标志
	int sat_num;//当前历元所观测到的卫星数量
	//此处定义动态数组，采用指针，因为是我在读取了数据，我才知道分配多少内存
	string satName;//字符数组测站名称存储
};

typedef struct navi_body
{
	//这个表示一个特定卫星的PRN号码
	string PRN;
	//这个表示卫星钟大数据time of clock
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
};


