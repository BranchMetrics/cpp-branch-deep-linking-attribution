import re, sys

# Substitutes replacement for \2 in any match
def update_file(path, regex, replacement):
    updated_lines = []
    with open(path) as f:
        for line in f.readlines():
            match = re.search(regex, line)
            if match:
                # TODO: Improve on this. Always requires 3 groups and replaces the second.
                line = match.group(1) + replacement + match.group(3) + "\n"
            updated_lines.append(line)

    with open(path, "w") as f:
        f.write("".join(updated_lines))


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

print("Bumping to " + full_version)

update_file("../../conanfile.py", r'^(\s*version\s*=\s*")(.*)(")', full_version)
update_file("../../CMakeLists.txt", r'^(\s*project\(root\s+VERSION\s+)(\d+\.\d+\.\d+)(.*)', reduced_version)
# BranchSDK/src/BranchIO/Version.h => reduced_version, split
# BranchSDK/Windows/BranchInstaller/Product.wxs => reduced_version
