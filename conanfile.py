from conans import ConanFile, CMake, tools
import os, shutil

class BranchioConan(ConanFile):
    # ----- Package metadata -----
    name = "BranchIO"
    # TODO(jdee): Set the version in one place and propagate it
    version = "0.1.0"
    license = "MIT"
    description = "Branch Metrics deep linking and attribution analytics C++ SDK"
    topics = (
        "windows",
        "macos",
        "linux",
        "c++",
        "branch",
        "metrics",
        "deeplink",
        "deep",
        "link",
        "attribution",
        "analytics"
    )
    author = "Branch Metrics, Inc <sdk-team@branch.io>"
    url = "https://github.com/BranchMetrics/cpp-branch-deep-linking-attribution"
    # TODO(jdee): Find an appropriate URL for this field, either docs or
    # marketing.
    # homepage = "https://docs.branch.io/cpp-sdk"

    # ----- Package settings and options -----
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "git_branch": "ANY", "source_folder": "ANY"}
    default_options = {"shared": False, "git_branch": None, "source_folder": None}
    generators = "cmake"
    exports_sources = "BranchIO"

    # ----- Package dependencies -----
    requires = "Poco/1.9.0@pocoproject/stable"
    build_requires = "gtest/1.8.1@bincrafters/stable"

    def source(self):
        if self.options.source_folder:
            # Install from local source (via rmake)
            folder = str(self.options.source_folder)
            self.output.info("Copying from source_folder " + folder)
            self.copyall(folder, ".", excludes=["build"])
        elif self.options.git_branch:
            # Install from a git branch in the repo
            git = tools.Git(folder=".")
            self.output.info("Checking out branch %s" % self.options.git_branch)

            # Allow specification of repo URL using BRANCHIO_GIT_URL env. var.
            git_url_env_var = os.environ.get('BRANCHIO_GIT_URL')
            git_url = git_url_env_var if git_url_env_var else self.url
            git.clone(git_url, branch=self.options.git_branch)
        else:
            # Install a release from a tag (default)
            tag_name = "v%s" % self.version
            self.output.info("Building from tag %s", tag_name)
            zip_name = "cpp-branch-deep-linking-%s.zip" % tag_name
            tools.download("%s/archive/%s" % (self.url, zip_name), zip_name)
            tools.unzip(zip_name)
            shutil.move("cpp-branch-deep-linking-%s" % tag_name, ".")
            os.unlink(zip_name)

    def build(self):
        library_type = "shared" if self.options.shared else "static"
        self.output.info("Building %s libraries" % library_type)

        cmake = CMake(self)

        # This is basically an optimization. According to the conan docs,
        # these flags are always on when doing conan install in the cache,
        # but this allows the user finer control if they want it.
        # https://docs.conan.io/en/latest/reference/conanfile/attributes.html#should-configure-should-build-should-install-should-test
        if self.should_configure:
            # conan build --configure
            build_shared_libs = self.options.shared
            cmake.configure(build_dir=".", defs={'BUILD_SHARED_LIBS': build_shared_libs, 'CMAKE_BUILD_TYPE': self.settings.build_type})
        if self.should_build:
            # conan build --build
            # build everything for now, including examples
            # we're probably about to run unit tests anyway
            cmake.build()
        if self.should_install:
            # conan build --install
            cmake.install()
        if self.should_test:
            # conan build --test
            cmake.test() # run unit tests

    def test(self):
        # TODO(jdee): This isn't necessarily the right idea. The idea is to use a project
        # that imports this one to test that we've been installed correctly.
        CMake(self).test()

    def package(self):
        self.copy("*.h", dst="include/BranchIO", src="BranchSDK/src/BranchIO")
        self.copy("*.h", dst="include/BranchIO/Event", src="BranchSDK/src/BranchIO/Event")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False, symlinks=True)
        self.copy("*.so*", dst="lib", keep_path=False, symlinks=True)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs.append("BranchIO")
        if self.settings.compiler == "Visual Studio":
            self.cpp_info.libs.extend(["kernel32", "advapi32"])
            if self.options.shared:
                self.cpp_info.defines.append("BRANCHIO_DLL")

    # Recursively copy src directory into dst directory
    # excludes is a list of patterns to exclude
    def copyall(self, src, dst, excludes=[]):
        # ignores is a function to be passed to copytree
        ignores = shutil.ignore_patterns(*excludes)
        all_files = os.listdir(src)
        # rejects is a set of all files matching the excludes
        rejected = ignores(src, all_files)
        files = [f for f in all_files if f not in rejected]

        for f in files:
            path = src + "/" + f
            if os.path.isdir(path) and os.path.exists(dst + "/" +f):
                # copytree fails if the destination directory already exists.
                # That's the purpose of this function. Call it recursively in
                # this case.
                copyall(path, dst + "/" +f, excludes=excludes)
            elif os.path.isdir(path):
                shutil.copytree(path, dst + "/" + f, ignore=ignores)
            else:
                shutil.copy(path, dst)
