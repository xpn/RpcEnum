#pragma once
#include <Windows.h>
#include <DbgHelp.h>
#include <string> 

using namespace std;

class Debug {
public:

	string *GetSymbolForAddresss(string name, void* base, DWORD size, void* addr);
	void SetSymbolPath(string path);
	Debug(int pid);

private:
	int pid;
	HANDLE pHandle;
	string symbolPath;
};