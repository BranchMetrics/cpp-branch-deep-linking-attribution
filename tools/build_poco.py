#! /usr/bin/env python
# build poco directly with cmake, the conan built binaries don't work for me.
# assumes the poco repo is checked out next to the Branch repo.

import json, os, shutil, subprocess
from os import makedirs

def configure(debug: bool, mt: bool):

    enable_mt = "-DPOCO_MT=OFF"
    if (mt):
        enable_mt = "-DPOCO_MT=ON"

    enable_debug = "-DCMAKE_BUILD_TYPE=Release"
    if (debug):
        enable_debug = "-DCMAKE_BUILD_TYPE=Debug"

    subprocess.run([
        "cmake",
        "..", 
        "-G",
        "Visual Studio 16 2019",
        "-A",
        "x64",
        enable_debug,
        "-DBUILD_SHARED_LIBS=OFF",
        enable_mt,
        "-DENABLE_CRYPTO=OFF",
        "-DENABLE_NETSSL=OFF",
        "-DENABLE_NETSSL_WIN=ON",
        "-DENABLE_ACTIVE_RECORD=OFF",
        "-DENABLE_DATA=OFF",
        "-DENABLE_DATA_MYSQL=OFF",
        "-DENABLE_DATA_ODBC=OFF",
        "-DENABLE_DATA_POSTGRESQL=OFF",
        "-DENABLE_DATA_SQLITE=OFF",
        "-DENABLE_ENCODINGS=OFF",
        "-DENABLE_JWT=OFF",
        "-DENABLE_MONGODB=OFF",
        "-DENABLE_REDIS=OFF",
        "-DENABLE_ZIP=OFF"])


def build(debug: bool):
    build_config = "Release"
    if (debug):
        build_config = "Debug"
    
    subprocess.run([
        "cmake",
        "--build",
        ".",
        "--config",
        build_config
    ])

def build_md():
    configure(False, False)
    build(False)

def build_mdd():
    configure(True, False)
    build(True)

def build_mt():
    configure(False, True)
    build(False)

def build_mtd():
    configure(True, True)
    build(True)

# path to poco cmake-build folder
build_folder = "cmake-build"
build_path = os.path.join("..", "poco", build_folder)

# remove old build products
shutil.rmtree(build_path, ignore_errors=True)
makedirs(build_path)

# build from within the cmake-build folder
os.chdir(build_path)

# build MD binaries
build_md()
build_mdd()

# build MT binaries
build_mt()
build_mtd()
