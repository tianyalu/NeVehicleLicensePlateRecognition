#ifndef UTILS_H
#define UTILS_H

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#else
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <vector>
#include <list>
#include <string>

using namespace std;

//获得目录下文件
void getFiles(const string folder, vector<string>& result);
#endif //UTILS_H
