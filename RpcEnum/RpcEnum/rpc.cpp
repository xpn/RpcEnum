#include "rpc.h"

using namespace std;

RPC_SYNTAX_IDENTIFIER DceRpcSyntaxUuid =
{
	{ 0x8a885d04,0x1ceb,0x11c9,{ 0x9f,0xe8,0x08,0x00,0x2b,0x10,0x48,0x60 } },
	{ 2,0 }
};

RPC::RPC(Process* p) {
	this->p = p;
}

/// Retrieves a list of methods for a provided RPC interface
void** RPC::GetMethodForInterface(RPC_INTERFACE_T *iface, DWORD *count) {
	RPC_DISPATCH_TABLE dispatchTable;
	MIDL_SERVER_INFO_T midlServerInfo;

	// Read the dispatch table from the interface
	if (this->p->ReadMemory(iface->RpcServerInterface.DispatchTable, (unsigned char*)& dispatchTable, sizeof(RPC_DISPATCH_TABLE)) != sizeof(RPC_DISPATCH_TABLE)) {
		return NULL;
	}

	void** functions = (void**)malloc(sizeof(void*) * dispatchTable.DispatchTableCount);
	if (functions == NULL) {
		return NULL;
	}

	// Retrieve function pointers from MIDL_SERVER_INFO 
	if (this->p->ReadMemory((void*)iface->RpcServerInterface.InterpreterInfo, (unsigned char*)& midlServerInfo, sizeof(MIDL_SERVER_INFO_T)) != sizeof(MIDL_SERVER_INFO_T)) {
		return NULL;
	}

	if (this->p->ReadMemory((void*)midlServerInfo.DispatchTable, (unsigned char*)functions, sizeof(void*) * dispatchTable.DispatchTableCount) != sizeof(void*) * dispatchTable.DispatchTableCount) {
		return NULL;
	}

	*count = dispatchTable.DispatchTableCount;

	return functions;
}

/// Retrieves a list of interfaces from the provided RPC_SERVER instance
RPC_INTERFACE_T* RPC::GetInterfaces(RPC_SERVER_T* rpcServer, DWORD* count) {

	void** ifaces;

	// Copy array of RPC_INTERFACE
	this->rpcInterfaces = (RPC_INTERFACE_T*)malloc(sizeof(RPC_INTERFACE_T) * rpcServer->InterfaceDict.NumberOfEntries);
	if (this->rpcInterfaces == NULL) {
		return NULL;
	}

	// Attempt to read interfaces
	ifaces = (void**)malloc(rpcServer->InterfaceDict.NumberOfEntries * sizeof(void*));
	if (this->p->ReadMemory((void*)rpcServer->InterfaceDict.pArray, (unsigned char*)ifaces, rpcServer->InterfaceDict.NumberOfEntries * sizeof(void*)) != rpcServer->InterfaceDict.NumberOfEntries * sizeof(void*)) {
		return NULL;
	}

	// Parse the interfaces
	for (int k = 0; k < rpcServer->InterfaceDict.NumberOfEntries; k++) {
		if (this->p->ReadMemory(ifaces[k], (unsigned char*)(&this->rpcInterfaces[k]), sizeof(RPC_INTERFACE_T)) != sizeof(RPC_INTERFACE_T)) {
			return NULL;
		}
		//Utils::PrintGUID(this->rpcInterfaces[k].RpcServerInterface.InterfaceId.Uuid);
	}


	*count = rpcServer->InterfaceDict.NumberOfEntries;

	return this->rpcInterfaces;
}

RPC_SERVER_T* RPC::GetGlobalRPCServer(void) {

	// Retrieve RPC_SERVER 
	return this->huntForGlobalRPCServer([](Process* p, RPC_INTERFACE_T** interfacePtrs, UINT count) -> bool {

		RPC_INTERFACE_T iface;

		// Iterate through interfaces
		for (int j = 0; j < count; j++) {

			if (p->ReadMemory(interfacePtrs[j], (unsigned char*)& iface, sizeof(RPC_INTERFACE_T)) == sizeof(RPC_INTERFACE_T)) {

				// Sanity check RPC_INTERFACE to look for a known transfer syntax GUID
				if (iface.RpcServerInterface.Length == sizeof(RPC_SERVER_INTERFACE_T) &&
					!memcmp(&DceRpcSyntaxUuid, &iface.RpcServerInterface.TransferSyntax, sizeof(DceRpcSyntaxUuid))
					) {
					return true;
				}
			}
		}

		return false;
	});
}

/// Hunts for RPC_SERVER and 
RPC_SERVER_T *RPC::huntForGlobalRPCServer(std::function<bool(Process* p, RPC_INTERFACE_T** interfacePtrs, UINT count)> validate) {

	IMAGE_SECTION_HEADER* dataSection = NULL;
	map<string, char*>* m;
	char* base;
	void** ifaces;
	RPC_INTERFACE_T iface;
	_RPC_SERVER_T* rpcServer;

	// Get loaded DLL's from the process
	m = this->p->GetLoadedModules();

	// Find rpcrt4.dll within the returned list
	if (m->count("RPCRT4.dll") == 0) {
		return NULL;
	}
	base = m->at("RPCRT4.dll");

	// Find .data section for RPCRT4.dll
	dataSection = this->p->FindSection((char*)base, ".data");
	if (dataSection == NULL) {
		return NULL;
	}

	// Align to virtual address
	base += dataSection->VirtualAddress;

	rpcServer = (RPC_SERVER_T*)malloc(sizeof(RPC_SERVER_T));
	if (rpcServer == NULL) {
		return NULL;
	}

	// Iterate through .data section to find RPC_SERVER
	for (int i = 0; i < dataSection->Misc.VirtualSize; i += 8) {

		ULONGLONG pointer;

		// Read a potential pointer to RPC_SERVER
		pointer = this->p->ReadQword((char*)base + i);

		// Attempt to read a potential RPC_SERVER object
		if (this->p->ReadMemory((void*)pointer, (unsigned char*)rpcServer, sizeof(RPC_SERVER_T)) != sizeof(RPC_SERVER_T)) {
			continue;
		}

		// Sanity check the interface dictionary
		if (rpcServer->InterfaceDict.NumberOfEntries <= MAX_SIMPLE_DICT_ENTRIES) {

			// Attempt to read interfaces
			ifaces = (void**)malloc(rpcServer->InterfaceDict.NumberOfEntries * sizeof(void*));
			if (ifaces == NULL) {
				return NULL;
			}

			if (this->p->ReadMemory((void*)rpcServer->InterfaceDict.pArray, (unsigned char*)ifaces, rpcServer->InterfaceDict.NumberOfEntries * sizeof(void*)) == rpcServer->InterfaceDict.NumberOfEntries * sizeof(void*)) {

				// Pass to validation function for further checks
				if (validate(this->p, reinterpret_cast<RPC_INTERFACE_T **>(ifaces), rpcServer->InterfaceDict.NumberOfEntries)) {

					// If validation passes, we have a RPC_SERVER instance which we can return
					free(ifaces);
					return rpcServer;
				}
			}
			free(ifaces);
		}
	}

	free(rpcServer);
	return NULL;
}