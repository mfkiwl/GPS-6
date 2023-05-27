#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include <math.h>
using namespace std;
class Matrix
{
public:
	// 默认构造函数
	Matrix(void);
	// 构造函数一
	Matrix(int row, int column);
	// 复制构造函数
	Matrix(const Matrix& m);

public:
	// 默认析构函数
	~Matrix(void);

public:	// 运算符
	// 赋值运算符
	Matrix& operator=(const Matrix& m);
	// 比较运算符
	bool operator==(const Matrix& m);
	bool operator!=(const Matrix& m);
	// 加/减运算符
	Matrix operator+(const Matrix& m);
	Matrix operator-(const Matrix& m);

	// 自加/减运算符
	Matrix& operator+=(const Matrix& m);
	Matrix& operator-=(const Matrix& m);

	// 单目运算符
	Matrix operator-();	// 取负数

	// 乘法运算符
	Matrix operator*(const Matrix& m);
	Matrix operator*(const double& s);


public:	// 属性变量
	// 设置(i,j)的值
	void setValue(int row, int column, double value) { _A[row][column] = value; }
	double getValue(int row, int column) const { return _A[row][column]; }

	// 设置行、列的值
	void setRow(const int row) { _row = row; }
	int getRow() const { return _row; }
	void setColunm(const int column) { _column = column; }
	int getColumn() const { return _column; }

	bool setUnitMatrix();

public:
	// 矩阵转置
	Matrix transpose();
	// 矩阵求逆
	Matrix inverse();	// 列主元素法
private:
	double** _A;
	int _row;	// 行
	int _column;	// 列
};