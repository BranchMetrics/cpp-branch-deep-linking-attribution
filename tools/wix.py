#! /usr/bin/env python
# setup for wix installer

import os, shutil, subprocess

def copyall(src, dst):   
    files = os.listdir(src)

    for f in files:
        path = os.path.join(src, f)
        destination = os.path.join(dst, f)
        if not os.path.isdir(path):
            shutil.copy(path, dst)

# paths relative to project root
stage_path = "stage"
installer_path = "BranchInstaller"

# generate wix wxs fragements
# requires wix installer tools to be installed
print("Generate Branch.wxs")
branch_wxs = os.path.join(stage_path, "Branch.wxs")
branch_wxs_tmp = os.path.join(stage_path, "Branch.wxs.tmp")

subprocess.run(["heat", "dir", stage_path, "-srd", "-cg", "BranchLibrary",  "-ag", "-sfrag", "-var", "var.ProjectDir", "-out", branch_wxs_tmp])

# correct directory name in Branch.wxs
# TODO: figure out how to set this correctly during heat generation
with open(branch_wxs_tmp,"rt") as fin:
    with open(branch_wxs, "wt") as fout:
        for line in fin:
            fout.write(line.replace("TARGETDIR", "INSTALLFOLDER"))
if os.path.isfile(branch_wxs_tmp):
    os.remove(branch_wxs_tmp)

# Copy BranchInstaller to stage. The generated wxs file assumes build products are in var.ProjectDir
# TODO: Can we set the path to something relative to var.ProjectDir? Then we can remove this copy step.
print("Copy Installer files to stage")
copyall(installer_path, stage_path)
