# Usage:
# python version_bump.py 1.2.2
#
# The version number is required and never inferred. Suffixes such as -beta.1 are supported.

import os, re, sys

# Substitutes replacement for \2 in any match
def update_file(path, regex, replacement):
    updated_lines = []
    with open(path) as f:
        for line in f.readlines():
            match = re.search(regex, line)
            if match:
                line = match.group(1) + replacement + match.group(3) + "\n"
            updated_lines.append(line)

    with open(path, "w") as f:
        f.write("".join(updated_lines))

def update_binary_file(path, regex, replacement, encoding):
    updated_lines = []
    with open(path, "rb") as f:
        text = f.read().decode(encoding)
        for line in text.split("\r\n"):
            match = re.search(regex, line)
            if match:
                line = match.group(1) + replacement + match.group(3)
            updated_lines.append(line)

    with open(path, "wb") as f:
        for l in updated_lines:
            f.write((l + "\r\n").encode(encoding))

if len(sys.argv) < 2:
    print("Please provide a new version number as a command-line argument, e.g. version_bump.py 1.2.1-alpha.3")
    exit(1)

full_version = sys.argv[1]

# Not all versioning systems involved support a suffix like -alpha.3, so remove it for those cases.
reduced_version = full_version
match = re.search(r'^(\d+\.\d+\.\d+)', full_version)
if not match:
    print("Version must begin with /^\d+\.\d+\.\d+/, e.g. 1.2.1-alpha.3. The suffix is optional.")
    exit(1)

# Now reduced_version is all digits. full_version may have a suffix.
reduced_version = match.group(0)
version_components = reduced_version.split(".")

print("Bumping to " + full_version)

# conan package version
update_file("conanfile.py", r'^(\s*version\s*=\s*")(.*)(")$', full_version)

# version in CMakeLists.txt
update_file("CMakeLists.txt", r'^(\s*project\(root\s+VERSION\s+)(\d+\.\d+\.\d+)(.*)$', reduced_version)

# version in MSI package
update_file("BranchInstaller/Product.wxs", r'^(\s*<Product.*Version=")(\d+\.\d+\.\d+)(".*)$', reduced_version)

# version components in Version.h
update_file("BranchSDK/src/BranchIO/Version.h", r'^(.*BRANCHIO_VERSION_MAJOR\s+)(\d+)(.*)$', version_components[0])
update_file("BranchSDK/src/BranchIO/Version.h", r'^(.*BRANCHIO_VERSION_MINOR\s+)(\d+)(.*)$', version_components[1])
update_file("BranchSDK/src/BranchIO/Version.h", r'^(.*BRANCHIO_VERSION_REVISION\s+)(\d+)(.*)$', version_components[2])
