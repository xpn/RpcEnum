import sys
import urllib2
import base64
import json
import os
import csv

# Maximum depth to search
MAX_DEPTH = 5

# CSV field names
fieldnames = ['type', 'module', 'name',
              'address', 'calledby', 'calledbymodule']

# Output files for CSV
fdext = open("log-ext.csv", 'a')
fdint = open("log-int.csv", 'a')
fdrpc = open("log-rpc.csv", 'a')

csvext = csv.DictWriter(fdext, delimiter=' ', quotechar='|',
                        quoting=csv.QUOTE_MINIMAL, fieldnames=fieldnames)
csvext.writeheader()

csvint = csv.DictWriter(fdint, delimiter=' ', quotechar='|',
                        quoting=csv.QUOTE_MINIMAL, fieldnames=fieldnames)
csvint.writeheader()

csvrpc = csv.DictWriter(fdrpc, delimiter=' ', quotechar='|',
                        quoting=csv.QUOTE_MINIMAL, fieldnames=fieldnames)
csvrpc.writeheader()

# Recursively called to parse called functions
def GetCalled(f, depth):

    # Traverse through to MAX_DEPTH
    if depth > MAX_DEPTH:
        return

    if f == None:
        return

    called = f.getCalledFunctions(getMonitor())

    if len(called) != 0:
        for c in called:
            if c.isExternal():
                csvext.writerow({'type': 'ExtFunction', 'module': c.getExternalLocation().getLibraryName(), 'name': c.getName(
                ), 'address': 0, 'calledby': f.getName(), 'calledbymodule': f.getProgram().getExecutablePath().replace("\\", "/")})

            else:
                csvint.writerow({'type': 'IntFunction', 'module': c.getProgram().getExecutablePath().replace("\\", "/"), 'name': c.getName(
                ), 'address': c.getEntryPoint(), 'calledby': f.getName(), 'calledbymodule': f.getProgram().getExecutablePath().replace("\\", "/")})

            # Parse further functions
            GetCalled(c, depth+1)


args = getScriptArgs()

for arg in args[0].split(","):
    i = int(arg)
    functionBase = currentProgram.getImageBase().add(i)
    f = getFunctionAt(functionBase)

    if f == None:
        # Weird bug in Ghidra where some functions aren't defined, let's check to see if this is the case for this
        if getSymbolAt(functionBase) != None:
            # Create our function
            print "[*] Creating function"
            f = createFunction(functionBase, None)
            print "[*] Function created with name %s" % (f.getName())

        else:
            print "[!] Could not find function %s" % (arg)
            continue
    else:
        print "[*] Processing function %s" % (f.getName())

    csvrpc.writerow({'type': 'RpcFunction', 'module': f.getProgram().getExecutablePath().replace(
        "\\", "/"), 'name': f.getName(), 'address': f.getEntryPoint(), 'calledby': '', 'calledbymodule': ''})
    GetCalled(f, 0)
