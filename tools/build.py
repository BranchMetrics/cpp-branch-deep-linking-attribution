#! /usr/bin/env python
# builds Branch SDK MD, MDd, MT, MTd binaries

import json, os, shutil, subprocess
from os import makedirs

# builds binary using rmake.bat
def build_binary(runtime, arch):
    subprocess.run(["rmake.bat", runtime, arch])

build_path = "./build/"

# remove cached build products
shutil.rmtree(build_path, ignore_errors=True)
makedirs(build_path)

# available runtimes, architectures
runtimes = ["MD", "MDd", "MT", "MTd"]
architectures = ["x86", "x64"]

for runtime in runtimes:
    for architecture in architectures:
        build_binary(runtime, architecture)
