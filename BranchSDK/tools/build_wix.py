#! /usr/bin/env python

"""
Script to build BranchSDK\Windows\BranchInstaller\Components.wxs for
Wix installer using the products of stage.py under build\*\stage.

Works best when run from the same directory (BranchSDK\tools), e.g.:

    cd BranchSDK\tools
    python build_wix.py
"""

# TODO: Don't necessarily need ET here, since we're just generating
# text, but this may be more flexible.

# TODO: Use heat.exe to replace all or most of this script.
# https://wixtoolset.org/documentation/manual/v3/overview/heat.html

import json, os, shutil
from uuid import uuid4
from xml.dom import minidom
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Comment, Element, SubElement

# -----
# ----- JSON Guid map
# -----

def guid_for_id(identifier):
    file_path = "component-ids.json"

    # load stored component IDs from disk if any
    component_ids = json.load(open(file_path, "r"))

    component_id = component_ids.get(identifier, None)
    if not component_id:
        component_id = str(uuid4())
        component_ids[identifier] = component_id
        f = open(file_path, "w")
        json.dump(component_ids, f)
        f.close()

    return component_id

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
def file_identifier(path):
    repo_root = os.path.abspath(os.path.dirname(__file__) + "../..")
    build_root = os.path.join(repo_root, "build")
    # Wix error message:
    #  Identifiers may contain ASCII characters A-Z, a-z, digits,
    #  underscores (_), or periods (.).  Every identifier must
    #  begin with either a letter or an underscore.
    # Also limited to 72 chars.
    file_id = path.replace(build_root + os.sep, "").replace(repo_root + os.sep, "").replace("BranchSDK" + os.sep, "").replace("~", "_").replace("-", "").replace(os.sep, ".")

    # Just truncate for now.
    return file_id[:72] if len(file_id) > 72 else file_id

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
    return SubElement(elem, "Component", {
        "Id": identifier,
        "Directory": directory,
        "Guid": guid_for_id(identifier)
        })

def make_file_elem(elem, identifier, source, name=None):
    if name:
        return SubElement(elem, "File", {"Id": identifier, "Name": name, "Source": source})
    else:
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
        # Hack: BranchIO.lib goes into its own manually-created ComponentGroup.
        # Just ignore that library for now, if we're in a folder that has it.
        # Otherwise, this has no effect.
        if f == "BranchIO.lib" or f == "BranchIO.pdb":
            continue

        fullpath = os.path.join(path, f)
        if os.path.isdir(fullpath):
            continue
        source = fullpath.replace(repo_root, "$(var.ProjectDir)\\..\\..\\..")
        make_file_elem(component, file_identifier(fullpath), source)

"""
Recursively generates a flat, unnested list of Component elements
within a component group.
"""
def wix_components(elem, paths, include_subdirs=True):
    if not type(paths) is list:
        paths = [paths]

    alldirs = []
    for path in paths:
        alldirs.append(path)
        if include_subdirs:
            alldirs = alldirs + all_subdirs(path)

    return [wix_component(elem, d) for d in alldirs]

# -----
# ----- Begin XML generation
# -----

root = Element("Wix", {"xmlns": "http://schemas.microsoft.com/wix/2006/wi"})
root.append(Comment(" Generated by build_wix.py. Do not edit by hand. "))

# os.path.abspath & os.path.join build the right kind of path for the os,
# using backslashes on Windows without typing \\ every time here.
build_root = os.path.abspath(os.path.dirname(__file__) + "../../build")

# Take all header files from Releasex64 arbitrarily, since they
# don't vary by build.
stage_root = os.path.join(build_root, "Releasex64", "stage")
include_root = os.path.join(stage_root, "include")
lib_root = os.path.join(stage_root, "lib")
license_root = os.path.join(stage_root, "licenses")

# -----
# ----- Directory tree Fragment.
# ----- Defines directories to be populated on install.
# -----

dir_fragment = SubElement(root, "Fragment")
source_dir = make_directory_elem(dir_fragment, "TARGETDIR", "SourceDir")
# C:\Program Files (x86)
program_files_folder = make_directory_elem(source_dir, "ProgramFilesFolder")
# C:\Program Files (x86)\Branch SDK
branch_sdk_install_folder = make_directory_elem(program_files_folder, "INSTALLFOLDER", "Branch SDK")

# Recursively build a nested tree of Directory elements to describe
# all header folders to be installed.
wix_directory(branch_sdk_install_folder, include_root)

lib_folder = make_directory_elem(branch_sdk_install_folder, "LIBFOLDER", "lib")
license_folder = make_directory_elem(branch_sdk_install_folder, "LICENSEFOLDER", "licenses")
docs_folder = make_directory_elem(branch_sdk_install_folder, "docs", "docs")
html_folder = make_directory_elem(docs_folder, "docs.html", "html")
make_directory_elem(html_folder, "docs.html.search", "search")
x64_lib_folder = make_directory_elem(lib_folder, "X64LIBFOLDER", "x64")
x86_lib_folder = make_directory_elem(lib_folder, "X86LIBFOLDER", "x86")
make_directory_elem(x64_lib_folder, "X64DEBUGLIBFOLDER", "Debug")
make_directory_elem(x86_lib_folder, "X86DEBUGLIBFOLDER", "Debug")
make_directory_elem(x64_lib_folder, "X64RELEASELIBFOLDER", "Release")
make_directory_elem(x86_lib_folder, "X86RELEASELIBFOLDER", "Release")

# -----
# ----- ComponentGroup list Fragment
# ----- Defines individual files to be installed.
# -----

cg_fragment = SubElement(root, "Fragment")
branch_headers = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchHeaders"})
branch_license = make_component_elem(branch_headers, "BranchLicense", "LICENSEFOLDER")
make_file_elem(branch_license, "BranchLicense", "$(var.ProjectDir)\\..\\..\\..\\LICENSE", "LICENSE.txt")

third_party_headers = SubElement(cg_fragment, "ComponentGroup", {"Id": "ThirdPartyHeaders"})
branch_libraries_x64 = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchLibrariesX64"})
third_party_libraries_x64 = SubElement(cg_fragment, "ComponentGroup", {"Id": "ThirdPartyLibrariesX64"})
branch_libraries_x86 = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchLibrariesX86"})
third_party_libraries_x86 = SubElement(cg_fragment, "ComponentGroup", {"Id": "ThirdPartyLibrariesX86"})
docs = SubElement(cg_fragment, "ComponentGroup", {"Id": "BranchDocs"})

# Use wix_components to generate the contents of each of the
# CG elements above.
wix_components(branch_headers, os.path.join(include_root, "BranchIO"))
wix_components(third_party_headers, os.path.join(include_root, "Poco"))
third_party_licenses = make_component_elem(third_party_headers, "ThirdPartyLicenses", "LICENSEFOLDER")
make_file_elem(third_party_licenses, "PocoLicense", "$(var.ProjectDir)\\..\\..\\..\\build\\Releasex64\\stage\\licenses\\LICENSE-Poco.txt")

wix_components(docs, os.path.abspath("..\\docs\\html"))

# The BranchIO.lib sits in the same lib folder with the third-party libs. This
# is as it should be, to avoid making devs pass multiple library paths at
# link time. Manually add it to its own ComponentGroups and ignore it in
# wix_component to allow the rest to be scooped up into the third-party
# CGs.
branch_debug_lib64_folder = make_component_elem(branch_libraries_x64,  "BranchLibrariesDebugX64", "X64DEBUGLIBFOLDER")
branch_release_lib64_folder = make_component_elem(branch_libraries_x64, "BranchLibrariesReleaseX64", "X64RELEASELIBFOLDER")
branch_debug_lib86_folder = make_component_elem(branch_libraries_x86, "BranchLibrariesDebugX86", "X86DEBUGLIBFOLDER")
branch_release_lib86_folder = make_component_elem(branch_libraries_x86, "BranchLibrariesReleaseX86", "X86RELEASELIBFOLDER")

make_file_elem(branch_debug_lib64_folder, "BranchDebugX64Library", "$(var.ProjectDir)\\..\\..\\..\\build\\Debugx64\\lib\\BranchIO.lib", "BranchIOmdd.lib")
make_file_elem(branch_debug_lib64_folder, "BranchDebugX64PDB", "$(var.ProjectDir)\\..\\..\\..\\build\\Debugx64\\stage\\lib\\BranchIO.pdb")
make_file_elem(branch_release_lib64_folder, "BranchReleaseX64Library", "$(var.ProjectDir)\\..\\..\\..\\build\\Releasex64\\lib\\BranchIO.lib", "BranchIOmd.lib")
make_file_elem(branch_debug_lib86_folder, "BranchDebugX86Library", "$(var.ProjectDir)\\..\\..\\..\\build\\Debug\\lib\\BranchIO.lib", "BranchIOmdd.lib")
make_file_elem(branch_debug_lib86_folder, "BranchDebugX86PDB", "$(var.ProjectDir)\\..\\..\\..\\build\\Debug\\stage\\lib\\BranchIO.pdb")
make_file_elem(branch_release_lib86_folder, "BranchReleaseX86Library", "$(var.ProjectDir)\\..\\..\\..\\build\\Release\\lib\\BranchIO.lib", "BranchIOmd.lib")

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

output_path = os.path.abspath(build_root + "/../BranchSDK/Windows/BranchInstaller/Components.wxs")
open(output_path, "w").write(output)
