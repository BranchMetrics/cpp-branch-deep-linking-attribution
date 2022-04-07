#! /usr/bin/env python
# builds Branch SDK MD, MDd, MT, MTd binaries

import json, os, shutil, subprocess
from os import makedirs

# Change directory to parent for rmake.bat and other files 
os.chdir( ".\\" );

# builds binary using rmake.bat
def build_binary(runtime, arch):
    # TODO: replace rmake.bat with python?
    subprocess.run(["rmake.bat", runtime, arch])

build_path = "build"

# remove cached build products
shutil.rmtree(build_path, ignore_errors=True)
makedirs(build_path)

# available runtimes, architectures
runtimes = ["MD", "MDd", "MT", "MTd"]
architectures = ["x86", "x64"]
#architectures = ["x64"]

for runtime in runtimes:
    for architecture in architectures:
        build_binary(runtime, architecture)

# Call stage.py script to create and copy everything into stage folder
subprocess.call(['python', '.\\tools\\stage.py'])

# Call wix.py script to setup for wix installer
subprocess.call(['python', '.\\tools\\wix.py'])

# Build installer
os.system("msbuild .\\stage\\BranchInstaller.sln /t:Build /p:Configuration=Debug")
