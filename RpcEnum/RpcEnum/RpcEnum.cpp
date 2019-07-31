#include <iostream>
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include <Windows.h>
#include <tlhelp32.h>
#include <fstream>

#include "utils.h"
#include "rpc.h"

using namespace std;
using json = nlohmann::json;

void foreach_process(std::function<void(int)> proc) {
	PROCESSENTRY32 process;
	BOOL ret;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);

	ret = Process32First(snapshot, &process);
	do {
		proc(process.th32ProcessID);
		ret = Process32Next(snapshot, &process);
	} while (ret == TRUE);
}

int main(int argc, char** argv)
{
	printf("RPCEnum by @_xpn_\n\t...huge thanks to RPCView\n\n");
	foreach_process([](int procId) {

		printf("Enumerating PID %d\n", procId);

		DWORD interfaceCount = 0;
		DWORD functionCount = 0;
		json j;

		Process* p = new Process(procId);
		if (p->Open() == 0) {
			return;
		}

		RPC rpcHunter(p);
		RPC_SERVER_T* rpcServer = rpcHunter.GetGlobalRPCServer();
		if (rpcServer == NULL) {
			return;
		}

		RPC_INTERFACE_T* rpcInterfaces = rpcHunter.GetInterfaces(rpcServer, &interfaceCount);
		if (rpcInterfaces == NULL || interfaceCount == 0) {
			return;
		}

		j["pid"] = procId;
		j["modules"] = json::array();

		for (int i = 0; i < interfaceCount; i++) {

			//Utils::PrintGUID(rpcInterfaces[i].RpcServerInterface.InterfaceId.Uuid);
			
			json o = json::object({
				{"uuid", Utils::ConvertGUID(rpcInterfaces[i].RpcServerInterface.InterfaceId.Uuid)},
				{"functions", json::array()}
				});

			void** functions = rpcHunter.GetMethodForInterface(&rpcInterfaces[i], &functionCount);
			if (functions == NULL) {
				continue;
			}

			for (int j = 0; j < functionCount; j++) {

				DllInfo* info = p->GetModuleByAddress(functions[j]);
				Debug debug(p->GetPid());

				string* modname = p->GetModuleFilename(info->base);
				o["module"] = info->name.c_str();
				o["module_filename"] = modname->c_str();
				o["module_base"] = (unsigned long long)info->base;

				string* out = debug.GetSymbolForAddresss(modname->c_str(), info->base, info->length, functions[j]);
				if (out != NULL) {
					o["functions"] += json::object({
						{"method", out->c_str()},
						{"addr", (unsigned long long)functions[j]}
						});

					//printf("RPC method found: %s!%s at address %p\n", info->name.c_str(), out->c_str(), functions[j]);
				}

			}

			j["modules"] += o;

		}

		char filename[MAX_PATH];
		snprintf(filename, MAX_PATH, "%d.txt", procId);
		std::ofstream jsonOutput(filename);
		jsonOutput << j.dump(1);
		jsonOutput.close();

	});

}