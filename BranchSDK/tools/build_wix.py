#! /usr/bin/env python

"""
Script to build Product.wxs for Wix installer using the output
of stage.py under build\*\stage.
"""

# TODO: Don't necessarily need ET here, since we're just generating
# text, but this may be more flexible.

import json, os, shutil
from uuid import uuid4
from xml.dom import minidom
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Comment, Element, SubElement

# -----
# ----- Utilities used below
# -----

# https://pymotw.com/2/xml/etree/ElementTree/create.html
def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

# Method used to generate a unique identifier for a file or directory.
# E.g
# /path/to/cpp-branch-deep-linking-attribution/build/Releasex64/stage/include -> INCLUDEFOLDER
# /path/to/cpp-branch-deep-linking-attribution/build/Releasex64/stage/include/BranchIO -> INCLUDEBRANCHIOFOLDER
# /path/to/cpp-branch-deep-linking-attribution/build/Releasex64/stage/include/BranchIO/Branch.h -> INCLUDEBRANCHIOBRANCHHEADER
# /path/to/cpp-branch-deep-linking-attribution/build/Releasex64/stage/lib/libPocoFoundationmd.lib -> LIBX64LIBPOCOFOUNDATIONMDLIBRARY
# /path/to/cpp-branch-deep-linking-attribution/build/Release/stage/lib/libPocoFoundationmd.lib -> LIBX86LIBPOCOFOUNDATIONMDLIBRARY
def file_identifier(path):
    split_path = os.path.splitdrive(path)
    path_components = split_path[1].split(os.sep)
    name = ""

    ignoring = True
    for component in path_components:
        # Once we find include or lib as a path component,
        # stop ignoring and concatenate the remaining components.
        if component == "include":
            ignoring = False

        # Deps have the same library names under Debugx64 and Debug as well as
        # under Releasex64 and Release.
        if component == "lib":
            ignoring = False
            if "Release" in path_components or "Debug" in path_components:
                name = name + "LIBX86"
            else:
                name = name + "LIBX64"
            continue

        if ignoring:
            continue

        name = name + component.upper()

    if os.path.isdir(path):
        return name + "FOLDER"

    # TODO: Make this more general with re?
    if path.endswith(".h"):
        return name.replace(".H", "HEADER")

    if path.endswith(".lib"):
        return name.replace(".LIB", "LIBRARY")

    return name

# Find all subdirectories to all depths. Returns a flat list, for
# a ComponentGroup.
def all_subdirs(path):
    if not os.path.isdir(path):
        return []

    # Now path must be a directory

    # List of full paths to all immediate children of path
    all_paths = [os.path.join(path, f) for f in os.listdir(path)]

    # Filter out only directories.
    # List of full paths to all immediate subdirectories of path
    all_dirs = [f for f in all_paths if os.path.isdir(f)]

    # Now all_nested is a list of lists. Flatten it out. There's no
    # possibility of repetition of elements.
    result = all_dirs
    for d in all_dirs:
        nested = all_subdirs(d)
        result = result + nested
    return result

# -----
# ----- Conveniences to make specific ET elements
# -----

def make_directory_elem(elem, identifier, name=None):
    if name:
        return SubElement(elem, "Directory", {"Id": identifier, "Name": name})
    else:
        return SubElement(elem, "Directory", {"Id": identifier})

def make_component_elem(elem, identifier, directory):
    # TODO: Generating a new UUID each time here. If that's an issue,
    # can put these in a JSON file and add if/when new directories
    # appear.
    return SubElement(elem, "Component", {
        "Id": identifier,
        "Directory": directory,
        "Guid": str(uuid4())
        })

def make_file_elem(elem, identifier, source):
    return SubElement(elem, "File", {"Id": identifier, "Source": source})

# -----
# ----- Methods to generate Wix XML from the contents of a given path
# -----

"""
Produces a nested tree of Directory elements like:

          <Directory Id="INCLUDEFOLDER" Name="include">
            <Directory Id="BRANCHIOINCLUDEFOLDER" Name="BranchIO">
              <Directory Id="BRANCHIOEVENTINCLUDEFOLDER" Name="Event" />
            </Directory>
          </Directory>
"""
def wix_directory(elem, path):
    basepath = os.path.basename(path)
    directory_id = file_identifier(path)
    wdir = make_directory_elem(elem, directory_id, basepath)

    files = os.listdir(path)
    for f in files:
        full_path = os.path.join(path, f)
        if not os.path.isdir(full_path):
            continue
        wix_directory(wdir, full_path)

    return wdir

"""
Generates a single Component element with all child File elements.
Used by wix_components.
"""
def wix_component(elem, path, identifier=None):
    # TODO: Determine a better Id (2nd arg to make_component_elem)
    # to use for each folder automatically.
    # The Directory property refers to the Id of one of the
    # Directory elements generated by wix_directory, so must use
    # the file_identifier for the path.
    # For now, also use that for the Id property.
    path_id = identifier or file_identifier(path)
    component = make_component_elem(elem, path_id, path_id)

    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    for f in os.listdir(path):
        # Hack: BranchIO.lib goes into its own manuall-created ComponentGroup.
        # Just ignore that library for now, if we're in a folder that has it.
        # Otherwise, this has no effect.
        if f == "BranchIO.lib":
            continue

        fullpath = os.path.join(path, f)
        if os.path.isdir(fullpath):
            continue
        source = fullpath.replace(repo_root, "$(var.ProjectDir)\\..\\..\\..")
        file_elem = make_file_elem(component, file_identifier(fullpath), source)

"""
Recursively generates a flat, unnested list of Component elements
within a component group.
"""
def wix_components(elem, paths, include_subdirs=True):
    if not type(paths) is list:
        paths = [paths]

    for path in paths:
        alldirs = [path]
        if include_subdirs:
            alldirs = alldirs + all_subdirs(path)
        return [wix_component(elem, d) for d in alldirs]

# -----
# ----- Begin XML generation
# -----

# -----
# ----- General header elements
# -----

root = Element("Wix", {"xmlns": "http://schemas.microsoft.com/wix/2006/wi"})
root.append(Comment(" Generated by build_wix.py. Do not edit by hand. "))
product = SubElement(root, "Product", {
    "Id": "*",
    "Name": "Branch C++ SDK for Windows",
    "Language": "1033",
    "Version": "0.1.0",
    "Manufacturer": "Branch Metrics, Inc.",
    "UpgradeCode": "29b1dc08-2190-48f0-bc3c-7455381f2156"
    })
SubElement(product, "Package", {
    "InstallerVersion": "200",
    "Compressed": "yes",
    "InstallScope": "perMachine"
    })
SubElement(product, "MajorUpgrade", {
    "DowngradeErrorMessage": "A newer version of [ProductName] is already installed."
    })
SubElement(product, "Media", {
    "Id": "1",
    "Cabinet": "cab1.cab",
    "EmbedCab": "yes"
    })
SubElement(product, "Icon", {
    "Id": "branch.ico",
    "SourceFile": "branch-badge-dark.ico"
    })
SubElement(product, "Property", {
    "Id": "ARPPRODUCTION",
    "Value": "branch.ico"
    })

feature = SubElement(product, "Feature", {
    "Id": "ProductFeature",
    "Title": "Branch Installer",
    "Level": "1"
    })

# -----
# ----- ComponentGroups to be included (defined below)
# -----

SubElement(feature, "ComponentGroupRef", {"Id": "BranchHeaders"})
SubElement(feature, "ComponentGroupRef", {"Id": "BranchLibrariesX64"})
SubElement(feature, "ComponentGroupRef", {"Id": "BranchLibrariesX86"})
SubElement(feature, "ComponentGroupRef", {"Id": "ThirdPartyHeaders"})
SubElement(feature, "ComponentGroupRef", {"Id": "ThirdPartyLibrariesX64"})
SubElement(feature, "ComponentGroupRef", {"Id": "ThirdPartyLibrariesX86"})

# os.path.abspath & os.path.join build the right kind of path for the os,
# using backslashes on Windows without typing \\ every time here.
build_root = os.path.abspath(os.path.dirname(__file__) + "../../build")

# Take all header files from Releasex64 arbitrarily, since they
# don't vary by build.
stage_root = os.path.join(build_root, "Releasex64", "stage")
include_root = os.path.join(stage_root, "include")
lib_root = os.path.join(stage_root, "lib")

# -----
# ----- Directory tree Fragment.
# ----- Defines directories to be populated on install.
# -----

dir_fragment = SubElement(root, "Fragment")
source_dir = make_directory_elem(dir_fragment, "TARGETDIR", "SourceDir")
# C:\Program Files (x86)
program_files_folder = make_directory_elem(source_dir, "ProgramFilesDir")
# C:\Program Files (x86)\Branch SDK
branch_sdk_install_folder = make_directory_elem(program_files_folder, "INSTALLFOLDER", "Branch SDK")

# Recursively build a nested tree of Directory elements to describe
# all header folders to be installed.
wix_directory(branch_sdk_install_folder, include_root)

lib_folder = make_directory_elem(branch_sdk_install_folder, "LIBFOLDER", "lib")
x64_lib_folder = make_directory_elem(lib_folder, "X64LIBFOLDER", "x64")
x86_lib_folder = make_directory_elem(lib_folder, "X86LIBFOLDER", "x86")
make_directory_elem(x64_lib_folder, "X64DEBUGLIBFOLDER", "Debug")
make_directory_elem(x86_lib_folder, "X86DEBUGLIBFOLDER", "Debug")
make_directory_elem(x64_lib_folder, "X64RELEASELIBFOLDER", "release")
make_directory_elem(x86_lib_folder, "X86RELEASELIBFOLDER", "release")

# -----
# ----- ComponentGroup list Fragment
# ----- Defines individual files to be installed.
# -----

cg_fragment = SubElement(root, "Fragment")
branch_headers = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchHeaders"})
third_party_headers = SubElement(cg_fragment, "ComponentGroup", {"Id": "ThirdPartyHeaders"})
branch_libraries_x64 = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchLibrariesX64"})
third_party_libraries_x64 = SubElement(cg_fragment, "ComponentGroup", {"Id": "ThirdPartyLibrariesX64"})
branch_libraries_x86 = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchLibrariesX86"})
third_party_libraries_x86 = SubElement(cg_fragment, "ComponentGroup", {"Id": "ThirdPartyLibrariesX86"})

# TODO: Use wix_components to generate the contents of each of the
# CG elements above.
wix_components(branch_headers, os.path.join(include_root, "BranchIO"))
wix_components(third_party_headers, [os.path.join(include_root, p) for p in ["Poco", "openssl"]])
# zlib headers are directly in the include_root. Don't include subdirs.
wix_components(third_party_headers, include_root, False)

# The BranchIO.lib sits in the same lib folder with the third-party libs. This
# is as it should be, to avoid making devs pass multiple library paths at
# link time. Manually add it to its own ComponentGroups and ignore it in
# wix_component to allow the rest to be scooped up into the third-party
# CGs.
branch_debug_lib64_folder = make_component_elem(branch_libraries_x64,  "BranchLibrariesDebugX64", "X64DEBUGLIBFOLDER")
branch_release_lib64_folder = make_component_elem(branch_libraries_x64, "BranchLibrariesReleaseX64", "X64RELEASELIBFOLDER")
branch_debug_lib86_folder = make_component_elem(branch_libraries_x86, "BranchLibrariesDebugX86", "X86DEBUGLIBFOLDER")
branch_release_lib86_folder = make_component_elem(branch_libraries_x86, "BranchLibrariesReleaseX86", "X86RELEASELIBFOLDER")

make_file_elem(branch_debug_lib64_folder, "BranchDebugX64Library", "$(var.ProjectDir)\\..\\..\\..\\build\Debugx64\stage\lib\BranchIO.lib")
make_file_elem(branch_release_lib64_folder, "BranchReleaseX64Library", "$(var.ProjectDir)\\..\\..\\..\\build\Releasex64\stage\lib\BranchIO.lib")
make_file_elem(branch_debug_lib86_folder, "BranchDebugX86Library", "$(var.ProjectDir)\\..\\..\\..\\build\Debug\stage\lib\BranchIO.lib")
make_file_elem(branch_release_lib86_folder, "BranchReleaseX86Library", "$(var.ProjectDir)\\..\\..\\..\\build\Release\stage\lib\BranchIO.lib")

x64_debug_lib_path = os.path.join(build_root, "Debugx64", "stage", "lib")
x64_release_lib_path = os.path.join(build_root, "Releasex64", "stage", "lib")
x86_debug_lib_path = os.path.join(build_root, "Debug", "stage", "lib")
x86_release_lib_path = os.path.join(build_root, "Release", "stage", "lib")

wix_component(third_party_libraries_x64, x64_debug_lib_path, "X64DEBUGLIBFOLDER")
wix_component(third_party_libraries_x64, x64_release_lib_path, "X64RELEASELIBFOLDER")
wix_component(third_party_libraries_x86, x86_debug_lib_path, "X86DEBUGLIBFOLDER")
wix_component(third_party_libraries_x86, x86_release_lib_path, "X86RELEASELIBFOLDER")

# -----
# ----- End XML generation
# -----

# -----
# ----- Generate output
# -----

output = prettify(root)

# Useful for debugging
# print(output)

output_path = os.path.abspath(build_root + "/../BranchSDK/Windows/BranchInstaller/Product.wxs")
open(output_path, "w").write(output)
