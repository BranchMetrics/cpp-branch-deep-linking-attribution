#! /usr/bin/env python
# check that binaries are built with correct options
# TODO: check for DEFAULTLIB, it's causing linker issues

import os, shutil, subprocess

# MD
#dumpbin /DIRECTIVES foo.lib | find /i "MSVCRT"
def check_MD(binary):
    result = subprocess.run(["dumpbin", "/DIRECTIVES", binary], capture_output=True, text=True)
    matches_MD = result.stdout.find("MSVCRT")
    matches_MDd = result.stdout.find("MSVCRTD")
    if matches_MD != -1 and matches_MDd == -1:
        print(binary + " is MD")
    else:
        print(binary + " is not MD")

# MDd
#dumpbin /DIRECTIVES foo.lib | find /i "MSVCRTD"
def check_MDd(binary):
    result = subprocess.run(["dumpbin", "/DIRECTIVES", binary], capture_output=True, text=True)
    matches_MDd = result.stdout.find("MSVCRTD")
    if matches_MDd != -1:
        print(binary + " is MDd")
    else:
        print(binary + " is not MDd")

# MT
#dumpbin /DIRECTIVES foo.lib | find /i "LIBCMT"
def check_MT(binary):
    result = subprocess.run(["dumpbin", "/DIRECTIVES", binary], capture_output=True, text=True)
    matches_MT = result.stdout.find("LIBCMT")
    matches_MTd = result.stdout.find("LIBCMTD")
    if matches_MT != -1 and matches_MTd == -1:
        print(binary + " is MT")
    else:
        print(binary + " is not MT")

# MTd
#dumpbin /DIRECTIVES foo.lib | find /i "LIBCMTD"
def check_MTd(binary):
    result = subprocess.run(["dumpbin", "/DIRECTIVES", binary], capture_output=True, text=True)
    matches_MTd = result.stdout.find("LIBCMTD")
    if matches_MTd != -1:
        print(binary + " is MTd")
    else:
        print(binary + " is not MTd")

# x86
#dumpbin /HEADERS foo.lib | find /i "machine (x86)"
def check_x86(binary):
    result = subprocess.run(["dumpbin", "/HEADERS", binary], capture_output=True, text=True)
    matches_x86 = result.stdout.find("machine (x86)")
    if matches_x86 != -1:
        print(binary + " is x86")
    else:
        print(binary + " is not x86")

# x64
#dumpbin /HEADERS foo.lib | find /i "machine (x64)"
def check_x64(binary):
    result = subprocess.run(["dumpbin", "/HEADERS", binary], capture_output=True, text=True)
    matches_x64 = result.stdout.find("machine (x64)")
    if matches_x64 != -1:
        print(binary + " is x64")
    else:
        print(binary + " is not x64")

def verbose_check(src):
    check_MD(src)
    check_MDd(src)
    check_MT(src)
    check_MTd(src)
    check_x86(src)
    check_x64(src)

verbose = True

md_src = os.path.join("build", "MD", "BranchSDK", "lib", "BranchIOmd.lib")
check_MD(md_src)
check_x86(md_src)

mt_src = os.path.join("build", "MT", "BranchSDK", "lib",  "BranchIOmt.lib")
check_MT(mt_src)
check_x86(mt_src)

md_64_src = os.path.join("build", "MD_64", "BranchSDK", "lib", "BranchIOmd.lib")
check_MD(md_64_src)
check_x64(md_64_src)

mt_64_src = os.path.join("build", "MT_64", "BranchSDK", "lib",  "BranchIOmt.lib")
check_MT(mt_64_src)
check_x64(mt_64_src)

mdd_src = os.path.join("build", "MDd", "BranchSDK", "lib", "BranchIOmdd.lib")
check_MDd(mdd_src)
check_x86(mdd_src)

mtd_src = os.path.join("build", "MTd", "BranchSDK", "lib",  "BranchIOmtd.lib")
check_MTd(mtd_src)
check_x86(mtd_src)

mdd_64_src = os.path.join("build", "MDd_64", "BranchSDK", "lib", "BranchIOmdd.lib")
check_MDd(mdd_64_src)
check_x64(mdd_64_src)

mtd_64_src = os.path.join("build", "MTd_64", "BranchSDK", "lib",  "BranchIOmtd.lib")
check_MTd(mtd_64_src)
check_x64(mtd_64_src)

print("Verbose checks:")
verbose_check(md_src)
verbose_check(mt_src)
verbose_check(md_64_src)
verbose_check(mt_64_src)
verbose_check(mdd_src)
verbose_check(mtd_src)
verbose_check(mdd_64_src)
verbose_check(mtd_64_src)
