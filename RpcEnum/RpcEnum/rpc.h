#pragma once
#include <windows.h>
#include <functional>
#include "process.h"
#include "rpcview.h"
#include "utils.h"
#include "debug.h"

using namespace std;

class Process;

class RPC {
public:
	RPC(Process* p);
	RPC_SERVER_T* GetGlobalRPCServer();
	RPC_INTERFACE_T* GetInterfaces(RPC_SERVER_T* rpcServer, DWORD *count);
	void** GetMethodForInterface(RPC_INTERFACE_T* iface, DWORD *count);

private:
	RPC_SERVER_T* huntForGlobalRPCServer(std::function<bool(Process* p, RPC_INTERFACE_T** interfacePtrs, UINT count) > validate);

	Process* p;
	RPC_SERVER_T rpcServer;
	RPC_INTERFACE_T* rpcInterfaces;
	DWORD rpcInterfaceCount;
};



