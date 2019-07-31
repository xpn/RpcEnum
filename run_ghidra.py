import os
import subprocess
import json

# Input dir containing JSON files
dir = 'output'

ghidraHeadless = "~/Applications/ghidra_9.0.4/support/analyzeHeadless"
projectPath = "~/Project"
projectName = "Windows.gpr"

# Arguments to pass to Ghidra
args = [ghidraHeadless, projectPath, projectName, "-postScript", "post_script.py", "functionshere", "-process", "modulehere", "-noanalysis"]

modules = []
count = 0

# Traverse through each JSON file
for filename in os.listdir(dir):
    with open(dir + "/" + filename) as f:
        jsonData = f.read()

    o = json.loads(jsonData)
    for mod in o["modules"]:
        funcList = ""
        for func in mod["functions"]:
            funcList += str(func["addr"] - mod["module_base"]) + ","

        funcList = funcList[:-1]

        # Pass function addresses as an argument
        args[5] = funcList

        # Pass the target module that our passed addresses exist within
        args[7] = mod["module"].lower()

        # Skip combase.dll
        if mod["module"] == "combase.dll":
            continue

        # Make sure we aren't just passing an empty set of functions
        if funcList == "":
            continue

        subprocess.call(args)
        print "============== Working on module %d of %d ==============" % (count+1, len(os.listdir(dir)))

    count += 1
