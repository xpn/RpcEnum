#include "process.h"

using namespace std;

Process::Process(int pid) {
	this->pid = pid;
}

/// Open a new process handle ready for processing
bool Process::Open() {

	this->ready = true;

	this->pHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (this->pHandle == 0) {
		this->pHandle = (HANDLE)0;
		this->ready = false;
	}

	return this->ready;
}

/// Return information on on a DLL within this process
struct DllInfo* Process::GetDllInfo(char* base) {
	MODULEINFO modInfo;

	struct DllInfo* info = new struct DllInfo;

	GetModuleInformation(this->pHandle, (HMODULE)base, &modInfo, sizeof(MODULEINFO));

	info->base = base;
	info->length = modInfo.SizeOfImage;

	return info;
}

/// Retrieves a list of Sections for a provided module address
vector<IMAGE_SECTION_HEADER> *Process::GetSections(char* base) {
	IMAGE_DOS_HEADER dosHeader;
	IMAGE_NT_HEADERS ntHeaders;
	IMAGE_SECTION_HEADER sectionHeader;
	WORD sig;
	vector<IMAGE_SECTION_HEADER>* sectionHeaders = new vector<IMAGE_SECTION_HEADER>();

	if (base == NULL) {
		return NULL;
	}

	sig = this->ReadWord(base);

	// Check signature
	if (sig != 'ZM') {
		return NULL;
	}

	// Read IMAGE_DOS_HEADER
	this->ReadMemory(base, reinterpret_cast<unsigned char*>(&dosHeader), sizeof(IMAGE_DOS_HEADER));

	// Read IMAGE_NT_HEADERS
	this->ReadMemory(base + dosHeader.e_lfanew, reinterpret_cast<unsigned char*>(&ntHeaders), sizeof(IMAGE_NT_HEADERS));

	// Validate PE signature
	if (ntHeaders.Signature != IMAGE_NT_SIGNATURE) {
		return NULL;
	}

	// Grab all section headers
	for (int i = 0; i < ntHeaders.FileHeader.NumberOfSections; i++) {
		this->ReadMemory(base + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS) + (sizeof(IMAGE_SECTION_HEADER)*i), reinterpret_cast<unsigned char*>(&sectionHeader), sizeof(IMAGE_SECTION_HEADER));
		sectionHeaders->push_back(sectionHeader);
	}

	return sectionHeaders;
}

/// Returns a section header within a DLL provided its base and section name
IMAGE_SECTION_HEADER *Process::FindSection(char* base, string name) {
	vector<IMAGE_SECTION_HEADER> *sections;

	// Get sections from the provided base
	sections = this->GetSections(base);
	
	// Iterate through sections and find 
	for (const auto& section : *sections) {
		if (strncmp((const char*)section.Name, name.c_str(), name.length()) == 0) {
			return new IMAGE_SECTION_HEADER (section);
		}
	}

	return NULL;
}

/// Retrieves a list of loaded modules (DLL)
map<string, char*> *Process::GetLoadedModules() {
	HMODULE hMods[1024];
	DWORD cbNeeded, i;
	LPCSTR filename;
	TCHAR szModName[MAX_PATH];

	map<string, char*>* modules = new map<string, char*>();

	if (EnumProcessModules(this->pHandle, hMods, sizeof(hMods), &cbNeeded))	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			if (GetModuleFileNameEx(this->pHandle, hMods[i], szModName,	sizeof(szModName) / sizeof(TCHAR))) {
				filename = PathFindFileName(szModName);
				modules->insert(pair<string, char*>(string((const char *)filename), (char *)hMods[i]));
			}
		}
	}

	return modules;
}

/// Return filename of DLL by address
string* Process::GetModuleFilename(void* base) {
	char szModName[MAX_PATH];

	if (GetModuleFileNameEx(this->pHandle, (HMODULE)base, szModName, sizeof(szModName) / sizeof(TCHAR))) {
		string* modpath = new string(szModName);
		return modpath;
	}

	return NULL;
}

/// Return information on a DLL by address
DllInfo* Process::GetModuleByAddress(void* addr) {
	map<string, char*>* mods = this->GetLoadedModules();

	for (std::map<string, char*>::iterator it = mods->begin(); it != mods->end(); ++it)
	{
		DllInfo* info = this->GetDllInfo(it->second);
		if (addr >= info->base && addr <= (char*)info->base + info->length) {
			// Found
			info->name = it->first;
			return info;
		}
	}
}

/// Read memory from process
SIZE_T Process::ReadMemory(void* addr, unsigned char* output, SIZE_T maxLen) {
	SIZE_T bytesRead = 0;

	ReadProcessMemory(this->pHandle, addr, output, maxLen, &bytesRead);

	return bytesRead;
}

/// Read a qword from process memory
ULONGLONG Process::ReadQword(void* addr) {
	ULONGLONG output;
	SIZE_T bytesRead = 0;

	ReadProcessMemory(this->pHandle, addr, &output, sizeof(ULONGLONG), &bytesRead);

	return output;
}

/// Read a dword from process memory
DWORD Process::ReadDword(void* addr) {
	DWORD output;
	SIZE_T bytesRead = 0;

	ReadProcessMemory(this->pHandle, addr, &output, sizeof(DWORD), &bytesRead);

	return output;
}

/// Read a word from process memory
WORD Process::ReadWord(void* addr) {
	WORD output;
	SIZE_T bytesRead = 0;

	ReadProcessMemory(this->pHandle, addr, &output, sizeof(WORD), &bytesRead);

	return output;
}

/// Read a byte from process memory
BYTE Process::ReadByte(void* addr) {
	BYTE output;
	SIZE_T bytesRead = 0;

	ReadProcessMemory(this->pHandle, addr, &output, sizeof(BYTE), &bytesRead);

	return output;
}

/// Return current PID of process
int Process::GetPid() {
	return this->pid;
}
