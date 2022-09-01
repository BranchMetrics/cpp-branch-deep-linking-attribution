#! /usr/bin/env python
# Collects build products into the stage directory

import json, os, shutil, subprocess
from os import makedirs

# Recursively copy src directory into dst directory
# excludes is a list of patterns to exclude
def copyall(src, dst, excludes=[]):
    # ignores is a function to be passed to copytree
    ignores = shutil.ignore_patterns(*excludes)
    all_files = os.listdir(src)
    # rejects is a set of all files matching the excludes
    rejected = ignores(src, all_files)
    files = [f for f in all_files if f not in rejected]

    for f in files:
        path = os.path.join(src, f)
        destination = os.path.join(dst, f)
         
        if not os.path.exists(dst):
            makedirs(dst)

        if os.path.isdir(path) and os.path.exists(destination):
            # copytree fails if the destination directory already exists.
            # That's the purpose of this function. Call it recursively in
            # this case.
            copyall(path, destination, excludes=excludes)
        elif os.path.isdir(path):
            shutil.copytree(path, destination, ignore=ignores)
        else:
            shutil.copy(path, dst)

# copy branch library
def copy_branch_libs(src, dst, target):
    print("Copy Branch SDK. " + target)
    copyall(os.path.join(src, target, "BranchSDK", "lib"), os.path.join(dst, "lib", target))

# recursively copy branch headers
def copy_branch_headers(src, dst):
    print("Copy Branch SDK headers. " + src)
    if not os.path.exists(src):
        return
    if not os.path.exists(dst):
        makedirs(dst)

    files = os.listdir(src)
    for f in files:
        path = os.path.join(src, f)
        destination = os.path.join(dst, f)
        if os.path.isdir(path):
            copy_branch_headers(path, destination)
        elif f.endswith(".h") or f.endswith(".H"):
            shutil.copy(path, dst)

def copy_branch_license(dst):
    print("Copy Branch License")
    shutil.copy("LICENSE", os.path.join(dst, "BRANCH_LICENSE.txt"))

# generate docs and copies them to stage
# requires doxygen to be installed
def build_docs(src, dst):
    print("Build documentation")
    shutil.rmtree(os.path.join(src, "docs"), ignore_errors=True)
    subprocess.run(["doxygen", "Doxyfile"], cwd=src)
    shutil.move(os.path.join(src, "docs"), os.path.join(dst, "docs"))

# paths relative to project root
sdk_path = "BranchSDK"
build_path = "build"
stage_path = "stage"

# create stage
shutil.rmtree(stage_path, ignore_errors=True)
makedirs(stage_path)

# copy source files needed by installer from build to stage
# requires SDK be built for all targets, use build.py

# by default only stage 64 bit binaries
#targets = ["MD_64", "MDd_64", "MT_64", "MTd_64"]
targets = ["MD", "MDd", "MD_64", "MDd_64", "MT", "MTd", "MT_64", "MTd_64"]

for target in targets:
    copy_branch_libs(build_path, stage_path, target)

copy_branch_headers(os.path.join(sdk_path, "src", "BranchIO"), os.path.join(stage_path, "include", "BranchIO"))
copy_branch_license(stage_path)

build_docs(sdk_path, stage_path)
