# RpcEnum and supporting files

This repo contains code referenced in the blog post [https://blog.xpnsec.com/analysing-rpc-with-ghidra-neo4j](https://blog.xpnsec.com/analysing-rpc-with-ghidra-neo4j).

* RpcEnum - The VS project used to enumerate RPC methods.
* post_script.py - Jython post-script used by Ghidra to generate call.
* run_ghidra.py - Pythong script used to parse RPCEnum JSON and invoke headless Ghidra.

*Note: dbghelp.dll and symsrv.dll from the "Debugging Tools for Windows" kit are required to be placed into the same directory as the RpcEnum.exe binary to allow PDB's be be processed*