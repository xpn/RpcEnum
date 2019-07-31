#include "debug.h"

Debug::Debug(int pid) {
	this->pid = pid;
	this->symbolPath = "srv*c:\\symbols\\*http://msdl.microsoft.com/download/symbols";
}

void Debug::SetSymbolPath(string path) {
	this->symbolPath = path;
}

string *Debug::GetSymbolForAddresss(string name, void *base, DWORD size, void* addr) {

	SYMBOL_INFO *symInfo;
	DWORD64 disp;

	this->pHandle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, this->pid);
	if (this->pHandle == 0) {
		return NULL;
	}
	if (!SymInitialize(this->pHandle, this->symbolPath.c_str(), FALSE)) {
		return NULL;
	}

	SYMSRV_INDEX_INFO syminfo;
	syminfo.sizeofstruct = sizeof(SYMSRV_INDEX_INFO);

	if (!SymSrvGetFileIndexInfo(name.c_str(), &syminfo, 0)) {
		return NULL;
	}
	char foundFile[1024];
	memset(foundFile, 0, sizeof(foundFile));

	// To use srv, you need dbghelp.dll, symsrv.dll in the same path
	SymSetSearchPath(this->pHandle, this->symbolPath.c_str());

	if (!SymFindFileInPath(this->pHandle, NULL, syminfo.pdbfile, reinterpret_cast<PVOID>(&syminfo.guid), syminfo.age, 0, SSRVOPT_GUIDPTR, foundFile, NULL, NULL)) {
		return NULL;
	}

	if (!SymLoadModule64(this->pHandle, NULL, foundFile, NULL, (DWORD64)base, 0)) {
		return NULL;
	}


	symInfo = (SYMBOL_INFO*)malloc(1000);
	symInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
	symInfo->MaxNameLen = 1000;

	if (!SymFromAddr(this->pHandle, (DWORD64)addr, &disp, symInfo)) {
		return NULL;
	}

	SymCleanup(this->pHandle);

	string* symname = new string(symInfo->Name);

	return symname;
}

