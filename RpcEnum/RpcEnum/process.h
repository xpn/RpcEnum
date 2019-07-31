#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <map> 
#include <string> 
#include <vector>
#include <shlwapi.h>

using namespace std;

struct DllInfo {
	string name;
	void* base;
	DWORD64 length;
};

class Process {
public:
	Process(int pid);
	bool Open();
	DllInfo* GetDllInfo(char* base);
	vector<IMAGE_SECTION_HEADER>* GetSections(char* base);
	map<string, char*>* GetLoadedModules();
	string* GetModuleFilename(void* base);
	DllInfo *GetModuleByAddress(void* base);
	SIZE_T ReadMemory(void* addr, unsigned char* output, SIZE_T maxLen);
	IMAGE_SECTION_HEADER *FindSection(char* base, string name);
	ULONGLONG ReadQword(void* addr);
	DWORD ReadDword(void* addr);
	WORD ReadWord(void* addr);
	BYTE ReadByte(void* addr);


	int GetPid();

private:
	bool ready = false;
	int pid = 0;
	HANDLE pHandle;
};

