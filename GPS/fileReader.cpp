#include<iostream>
#include<cmath>
#include <iomanip>
#include "fileReader.h"
#include <string.h>
#include <sstream>
#include <fstream>
#include <string>

//O文件的读取
void fileReader::readFile_O(struct obs_head& ohead, struct obs_epoch& oepoch, string broadcastFile, fileReader& filereader)
{
	//读取数据
	string fileName = broadcastFile;

	/*定义文件流对象*/
	fstream file;

	//读入文件
	file.open(fileName);

	//判断是否找到文件
	if (!file)
	{
		std::cout << "文件地址错误，请重新寻找文件" << endl;
	}

	//临时存储数据的每一行，不用字符数组，不然后面不好断行
	string buf;
	//文件总行数
	int allHangNum = 0;
	//while循环主要是头部数据的读取
	while (1)
	{
		//获取到文件的一行
		getline(file, buf);
		//获取到一行数据之后，可以用总行数来记录一下
		allHangNum++;
		//如果找到了"END OF HEADER"就跳出循环,结束对行的记录
		if (buf.find("END OF HEADER") != std::string::npos)
			break;
		//对于头结构体进行数据提取
		if (buf.find("RINEX VERSION / TYPE") == 60)
		{
			//从字符串中得到版本号，同时一定要注意转换成double类型，因为结构体中定义的是double类型
			ohead.ver = std::stod(buf.substr(5, 9));
			//从字符串中得到观测值类型，例如M就代表混合观测值
			ohead.type = buf.substr(20, 36);
			//输出数据
			std::cout << "版本号：" << ohead.ver << endl;
			std::cout << "观测值类型：" << ohead.type << endl;
		}

		//获取粗略坐标X,Y,Z
		if (buf.find("APPROX POSITION XYZ") == 60)
		{
			//从字符串中获取X
			ohead.apX = std::stod(buf.substr(2, 14));
			//从字符串中获取Y
			ohead.apY = std::stod(buf.substr(16, 28));
			//从字符串中获取Z
			ohead.apZ = std::stod(buf.substr(29, 42));
			//输出粗略坐标
			std::cout << "粗略坐标:" << ohead.apX << "   " << ohead.apY << "   " << ohead.apZ << endl;
		}
		//获取观测值数目与类型数据
		if (buf.find("# / TYPES OF OBSERV") == 60)
		{
			//获取观测值数目
			ohead.obstypenum = std::stod(buf.substr(5, 6));
			string buf1 = buf.substr(10, 50);
			string tembuf = filereader.trim(buf1);
			ohead.obstype = tembuf;
			std::cout << "观测类型数目：" << ohead.obstypenum << endl << "观测值类型：" << ohead.obstype << endl;
		}
		//获取历元间隔
		if (buf.find("INTERVAL") == 60)
		{
			//获取观测历元间隔
			ohead.interval = std::stod(buf.substr(4, 11));
			std::cout << "观测历元间隔:" << ohead.interval << endl;
		}

		//获取观测时刻与时间系统
		if (buf.find("TIME OF FIRST OBS") == 60)
		{
			//获取时刻年月日等
			ohead.f_y = std::stod(buf.substr(2, 6));
			ohead.f_m = std::stod(buf.substr(10, 12));
			ohead.f_d = std::stod(buf.substr(17, 19));
			ohead.f_h = std::stod(buf.substr(24, 25));
			ohead.f_min = std::stod(buf.substr(30, 31));
			ohead.f_sec = std::stod(buf.substr(34, 43));
			//将时间系统写入char
			for (int i = 0; i < 5; i++)
			{
				ohead.tsys[i] = buf[i + 48];
			}
			std::cout << "观测时刻：" << ohead.f_y << " " << ohead.f_m << " " << ohead.f_d << " " << ohead.f_h << " " << ohead.f_min << " " << ohead.f_sec << endl;
			std::cout << "时间系统：";
			//输出时间系统
			for (int i = 0; i < 5; i++)
			{
				std::cout << ohead.tsys[i];
			}
			std::cout << endl;
		}
	}

	//获取观测站个数与属于哪些观测站
	//从哪一行开始读取观测值文件呢
	int start = allHangNum+1;
	//读取第25行的数据
	string buf1 = filereader.getLine(broadcastFile, start);
	//此处还需要优化，如果为个位数呢？
	oepoch.sat_num = std::atoi(buf1.substr(30, 32).c_str());
	std::cout << "卫星数目：" << oepoch.sat_num << endl;


	//读取观测时间
	oepoch.y= std::atoi(buf1.substr(1, 3).c_str());
	oepoch.m= std::atoi(buf1.substr(4, 6).c_str());
	oepoch.d= std::atoi(buf1.substr(8, 9).c_str());
	string strh = buf1.substr(10, 13);
	oepoch.h= std::atoi(filereader.trim(strh).c_str());
	string strm = buf1.substr(13, 16);
	oepoch.min = std::atoi(filereader.trim(strm).c_str());
	string strSec = buf1.substr(16, 10);
	oepoch.sec = std::stod(filereader.trim(strSec).c_str());
	//卫星数目占几行？
	int ln;
	if((oepoch.sat_num % 12)==0)
	{
		ln = oepoch.sat_num / 12;
	}
	else
	{
		ln = (oepoch.sat_num - (oepoch.sat_num % 12)) / 12 + 1;
	}

	//卫星站的字符串
	string str = "";
	for (int i = 25; i < 25 + ln; i++) {
		string tembufer = filereader.getLine(broadcastFile, i);
		str = str + tembufer.substr(32, 69);
	}
	oepoch.satName = str;
	std::cout << "卫星站字符串:" << oepoch.satName << endl;

	//真正的观测值数据的读取
	//for (int i = 0; i < oepoch.sat_num; i++)
	for (int i = 0; i <25; i++)
	{
		int x = 25 + ln + i * 2;
		int y = 25 + ln + 1 + i * 2;
		string obs1Line = filereader.getLine(broadcastFile,x);
		string obs1 = obs1Line.substr(1, 13);
		//表示从第17个开始，往后延伸14个
		string obs2 = obs1Line.substr(17, 14);
		string obs3 = obs1Line.substr(33, 14);
		string obs4 = obs1Line.substr(48, 16);
		string obs5 = obs1Line.substr(65, 12);

		//首先如果是全部都是空格字符串，需要全部移除空格
		string removeObs1 = filereader.trim(obs1);
		//移除空格之后还需要将它进行填补为“10000000”
		removeObs1 = filereader.trim(removeObs1);
		//然后将填补完成的数据写入二维数组
		filereader.observation[i][0] = std::stod(removeObs1);

		string removeObs2 = filereader.trim(obs2);
		removeObs2 = filereader.trim(removeObs2);
		filereader.observation[i][1] = std::stod(removeObs2);


		string removeObs3 = filereader.trim(obs3);
		removeObs3 = filereader.trim(removeObs3);
		filereader.observation[i][2] = std::stod(removeObs3);


		string removeObs4 = filereader.trim(obs4);
		removeObs4 = filereader.trim(removeObs4);
		filereader.observation[i][3] = std::stod(removeObs4.c_str());


		string removeObs5 = filereader.trim(obs5);
		removeObs5 = filereader.trim(removeObs5);
		filereader.observation[i][4] = std::stod(removeObs5);

		string obs2Line = filereader.ReadLine(filereader.filepath, y);

		//这个地方非常德关键
		//为什么要这样做呢
		if (obs2Line.size() < 31)
		{
				string str2(20, ' ');
				obs2Line.append(str2);
		}
		if (obs2Line.size() < 21)
		{
			string str2(30, ' ');
			obs2Line.append(str2);
		}

		string obs6 = obs2Line.substr(1, 14);
		string obs7 = obs2Line.substr(22, 9);
		string obs8 = obs2Line.substr(40, 6);

		string removeObs6 = filereader.trim(obs6);
		removeObs6 = filereader.trim(removeObs6);
		filereader.observation[i][5] = std::stod(removeObs6);

	
		string removeObs7 = filereader.trim(obs7);
		removeObs7 = filereader.trim(removeObs7);
		filereader.observation[i][6] = std::stod(removeObs7);

		
		string removeObs8 = filereader.trim(obs8);
		removeObs8 = filereader.trim(removeObs8);
		filereader.observation[i][7] = std::stod(removeObs8);

	}
	//真正观测值的输出
	//for (int i = 0; i < oepoch.sat_num; i++)
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			std::cout << filereader.observation[i][j]<<"  ";
		}
		std::cout << endl;
	}
}
//N文件的读取
void fileReader::readFile_N(std::vector<navi_body>& nbody, string navigationFile, fileReader& filereader,int allHangNum_n)
{
	

	for (int m = 0; m < nbody.size(); m++)
	{
		int start = allHangNum_n+m*8;
		string buf1 = filereader.getLine(navigationFile, start);
		//此处还需要优化，如果为个位数呢？


		//获取PRN号
		string strPRN = buf1.substr(0, 3);
		nbody[m].PRN = strPRN;
		std::cout << "卫星PRN号：" << nbody[m].PRN << endl;

		//获取时间年
		string strTOC_year = buf1.substr(4, 4);
		nbody[m].TOC_year = std::atoi(strTOC_year.c_str());
		std::cout << "年：" << nbody[m].TOC_year << endl;

		//获取时间月
		string strTOC_month = buf1.substr(9, 2);
		nbody[m].TOC_month = std::atoi(strTOC_month.c_str());
		std::cout << "月：" << nbody[m].TOC_month << endl;

		//获取时间日
		string strTOC_day = buf1.substr(12, 2);
		nbody[m].TOC_day = std::atoi(strTOC_day.c_str());
		std::cout << "日：" << nbody[m].TOC_day << endl;

		//获取时间时
		string strTOC_hour = buf1.substr(15, 2);
		nbody[m].TOC_hour = std::atoi(strTOC_hour.c_str());
		std::cout << "时：" << nbody[m].TOC_hour << endl;

		//获取时间分
		string strTOC_minute = buf1.substr(18, 2);
		nbody[m].TOC_minute = std::atoi(strTOC_minute.c_str());
		std::cout << "分：" << nbody[m].TOC_minute << endl;

		//获取时间秒
		string strTOC_second = buf1.substr(21, 2);
		nbody[m].TOC_second = std::atoi(strTOC_second.c_str());
		std::cout << "秒：" << std::fixed << std::setprecision(1) << nbody[m].TOC_second << endl;

		string bufx = buf1;
		//TOC时间的获取
		for (int i = 0; i < 3; i++)
		{

			int left;
			int right;
			left = 23 + i * 19;
			right = 23 + (i + 1) * 19;

			//获取底数
			string DI = bufx.substr(left, 15);
			string DI_remove;
			if (bufx.substr(left, 1) == "-")
			{
				 DI_remove = DI;
			}
			else {
				 DI_remove = bufx.substr(left+1, 14);
			}
			//底数转数字
			double numDI;
			numDI = std::stod(DI_remove);

			//获取指数
			string ZHI = bufx.substr(right - 3, 3);
			string ZHI_remove = filereader.trim(ZHI);

			//指数转数字
			double numZHI;
			numZHI = std::stod(ZHI_remove);

			//计算实际数字
			nbody[m].TOC[i] = numDI * pow(10, numZHI);
		}


		//读取主干部分6行4列的数据
		for (int i = 0; i < 6; i++) {
			int readStart = start + i + 1;
			string bufx= filereader.getLine(navigationFile, readStart);
			for (int j = 0; j < 4; j++) {
				int left;
				int right;
				left = 4 + j * 19;
				right = 4 + (j + 1) * 19;

				//获取底数
				string DI = bufx.substr(left, 15);
				string DI_remove;
				if (bufx.substr(left, 1) == "-")
				{
					DI_remove = DI;
				}
				else {
					DI_remove = bufx.substr(left + 1, 14);
				}
				//底数转数字
				double numDI;
				numDI = std::stod(DI_remove);
				//获取指数
				string ZHI = bufx.substr(right - 3, 3);
				string ZHI_remove = filereader.trim(ZHI);

				//指数转数字
				double numZHI;
				numZHI = std::stod(ZHI_remove);

				nbody[m].satliteData[i][j] = numDI * pow(10, numZHI);
			}
		}
	}
			//最后两行是有关电报文发送时刻与拟合区间的数据
			//最后两行懒得读了，感觉没用，等我感觉有用的时候再读
}
int fileReader::getHeaderNum(string navigationFile)
{
	//读取数据
	string fileName = navigationFile;

	/*定义文件流对象*/
	fstream file_n;

	//读入文件
	file_n.open(fileName);

	//判断是否找到文件
	if (!file_n)
	{
		std::cout << "文件地址错误，请重新寻找文件" << endl;
	}
	//临时存储数据的每一行，不用字符数组，不然后面不好断行
	string buf;
	//文件总行数
	int allHangNum_n = 0;
	//while循环主要是头部数据的读取
	while (1)
	{
		//获取到文件的一行
		getline(file_n, buf);
		//获取到一行数据之后，可以用总行数来记录一下
		allHangNum_n++;
		//如果找到了"END OF HEADER"就跳出循环,结束对行的记录
		if (buf.find("END OF HEADER") != std::string::npos)
			break;
	}

	return allHangNum_n;
}
string fileReader::getLine(const std::string& fileName, int lineNumber)
{
	std::ifstream file(fileName);
	std::string line;
	int currentLine = 0;

	while (std::getline(file, line)) {
		++currentLine;
		if (currentLine == lineNumber) {
			file.close();
			return line;
		}
	}

	file.close();
	return "";
}
std::vector<caculatePosition> fileReader::nbodyTocacupos(std::vector<navi_body>& nbody, std::vector<caculatePosition>& ca)
{
	for(int k=0;k<ca.size();k++)
	{ 
	ca[k].PRN = nbody[k].PRN;

	for (int i = 0; i < 3; i++) {
		ca[k].TOC[i] = nbody[k].TOC[i];
	}

	for (int i = 0; i < 6; i++) {
		for (int j =0 ; j < 4; j++) {
			ca[k].satliteData[i][j] = nbody[k].satliteData[i][j];
		}
	}

	for (int i = 0; i < 2; i++) {
		ca[k].satliteDataElse[i] = nbody[k].satliteDataElse[i];
	}

	ca[k].TOC_year = nbody[k].TOC_year;
	ca[k].TOC_month = nbody[k].TOC_month;
	ca[k].TOC_day = nbody[k].TOC_day;
	ca[k].TOC_hour = nbody[k].TOC_hour;
	ca[k].TOC_minute = nbody[k].TOC_minute;
	ca[k].TOC_second = nbody[k].TOC_second;
	}
	return ca;
}
void fileReader::nbodyTonbody(struct navi_body& nbody1, struct navi_body& nbody2) {

	nbody1.PRN = nbody2.PRN;

	for (int i = 0; i < 3; i++) {
		nbody1.TOC[i] = nbody2.TOC[i];
	}

	for (int i = 0; i < 6; i++) {
		for (int j =0 ; j < 4; j++) {
			nbody1.satliteData[i][j] = nbody2.satliteData[i][j];
		}
	}

	for (int i = 0; i < 2; i++) {
		nbody1.satliteDataElse[i] = nbody2.satliteDataElse[i];
	}

	nbody1.TOC_year = nbody2.TOC_year;
	nbody1.TOC_month = nbody2.TOC_month;
	nbody1.TOC_day = nbody2.TOC_day;
	nbody1.TOC_hour = nbody2.TOC_hour;
	nbody1.TOC_minute = nbody2.TOC_minute;
	nbody1.TOC_second = nbody2.TOC_second;
}
//获取PRN
void fileReader::getPRN(obs_epoch& oepoch, int num, std::vector<int>& vec)
{
	for (int i = 0; i < num; i++) {
		string sub = oepoch.satName.substr(3*i + 1, 2);
		vec.push_back(std::stoi(sub));// 存储指令编号
	}
}

double fileReader::getobsL1(fileReader fr, int col)
{
	return fr.observation[col][2];
}

void fileReader::setobsL1(fileReader& fr, int col, double N)
{
	fr.observation[col][2] = N;
}

//将对应的PRN的卫星类型存储起来，方便后续找卫星
void fileReader::getSatliteType(obs_epoch& oepoch, int num, std::vector<char>& vec)
{
	const char* cstr = oepoch.satName.c_str();
	for (int i = 0; i < num; i++) {
		vec.push_back(cstr[3 * i]);
	}
}
//获取粗略坐标
int fileReader::getInitPosition(string FileName)
{
	//读取数据
	string fileName = FileName;

	/*定义文件流对象*/
	fstream file;

	//读入文件
	file.open(fileName);

	//判断是否找到文件
	if (!file)
	{
		std::cout << "文件地址错误，请重新寻找文件" << endl;
	}

	//临时存储数据的每一行，不用字符数组，不然后面不好断行
	string buf;
	return 0;
}

//去除string中的字符串
string fileReader::trim(string& s)
{
	int index= 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}
	if (s.empty())
	{
		s = "10000000";
	}
	return s;
}

//读取特定行的函数实现
int fileReader::CountLines(string filename)
{
	ifstream ReadFile;
	int n = 0;
	string tmp;
	ReadFile.open(filename.c_str());//ios::in 表示以只读的方式读取文件
	if (ReadFile.fail())//文件打开失败:返回0
	{
		return 0;
	}
	else//文件存在
	{
		while (getline(ReadFile, tmp, '\n'))
		{
			n++;
		}
		ReadFile.close();
		return n;
	}
}
string fileReader::ReadLine(string filename, int line)
{
	int lines, i = 0;
	string temp;
	fstream file;
	file.open(filename.c_str());
	lines = CountLines(filename);

	if (line <= 0)
	{
		return "Error 1: 行数错误，不能为0或负数。";
	}
	if (file.fail())
	{
		return "Error 2: 文件不存在。";
	}
	if (line > lines)
	{
		return "Error 3: 行数超出文件长度。";
	}
	while (getline(file, temp) && i < line - 1)
	{
		i++;
	}
	file.close();
	return temp;
}
