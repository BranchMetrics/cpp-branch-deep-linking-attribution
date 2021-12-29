#! /usr/bin/env python
# builds Branch SDK MD, MDd, MT, MTd binaries

import json, os, shutil, subprocess
from os import makedirs

# fix the names of the MD and MT binaries
# TODO: figure out why the cmake postfix isn't working for Release builds
def fix_x64_lib_names():
    md_64_src = os.path.join("build", "MD_64", "BranchSDK", "lib", "BranchIO.lib")
    md_64_dst = os.path.join("build", "MD_64", "BranchSDK", "lib",  "BranchIOmd.lib")
    shutil.move(md_64_src, md_64_dst)

    mt_64_src = os.path.join("build", "MT_64", "BranchSDK", "lib",  "BranchIO.lib")
    mt_64_dst = os.path.join("build", "MT_64", "BranchSDK", "lib",  "BranchIOmt.lib")
    shutil.move(mt_64_src, mt_64_dst)

def fix_x86_lib_names():
    md_src = os.path.join("build", "MD", "BranchSDK", "lib", "BranchIO.lib")
    md_dst = os.path.join("build", "MD", "BranchSDK", "lib",  "BranchIOmd.lib")
    shutil.move(md_src, md_dst)

    mt_src = os.path.join("build", "MT", "BranchSDK", "lib",  "BranchIO.lib")
    mt_dst = os.path.join("build", "MT", "BranchSDK", "lib",  "BranchIOmt.lib")
    shutil.move(mt_src, mt_dst)

# builds binary using rmake.bat
def build_binary(runtime, arch):
    # TODO: replace rmake.bat with python?
    subprocess.run(["rmake.bat", runtime, arch])

# TODO: is this necessary? It's very slow.
def clear_conan():
    subprocess.run(["conan", "remove", "-f", "*"])

build_path = "build"

# remove cached build products
shutil.rmtree(build_path, ignore_errors=True)
makedirs(build_path)

# available runtimes, architectures
runtimes = ["MD", "MDd", "MT", "MTd"]
#architectures = ["x86", "x64"]
architectures = ["x64"]

for runtime in runtimes:
    for architecture in architectures:
        #clear_conan()
        build_binary(runtime, architecture)

# workaround an issue with MD and MT filenames
fix_x64_lib_names()
#fix_x86_lib_names()