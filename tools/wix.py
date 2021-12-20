#! /usr/bin/env python
# setup for wix installer

import os, subprocess

# paths relative to project root
stage_path = "stage"
installer_path = "BranchInstaller"

# generate wix wxs fragements
# requires wix installer tools to be installed
branch_wxs = os.path.join(installer_path, "Branch.wxs")
subprocess.run(["heat", "dir", stage_path, "-srd", "-cg", "BranchLibrary",  "-ag", "-sfrag", "-var", "var.ProjectDir", "-out", branch_wxs])