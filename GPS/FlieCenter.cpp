#include "FlieCenter.h"
#include <iostream>
#include <fstream>
using namespace std;
int openFile(char path) {
    int x, sum = 0;
    ifstream srcFile("path", ios::in); //以文本模式打开in.txt备读
    if (!srcFile) { //打开失败
        cout << "error opening source file." << endl;
        return 0;
    }
    ofstream destFile("out.txt", ios::out); //以文本模式打开out.txt备写
    if (!destFile) {
        srcFile.close(); //程序结束前不能忘记关闭以前打开过的文件
        cout << "error opening destination file." << endl;
        return 0;
    }
    //可以像用cin那样用ifstream对象
    while (srcFile >> x) {
        sum += x;
        //可以像 cout 那样使用 ofstream 对象
        destFile << x << " ";
    }
    cout << "sum：" << sum << endl;
    destFile.close();
    srcFile.close();
    return 0;
}