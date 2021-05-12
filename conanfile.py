from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration
import os, shutil

class BranchioConan(ConanFile):
    scm = {
        "type": "git",
        "url": "https://github.com/BranchMetrics/cpp-branch-deep-linking-attribution",
        "revision": "auto"
    }

    # ----- Package metadata -----
    name = "BranchIO"
    version = "1.2.2"
    license = "MIT"
    description = "Branch Metrics deep linking and attribution analytics C++ SDK"
    topics = (
        "windows",
        "win32",
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
    homepage = "https://help.branch.io/developers-hub/docs/windows-cpp-sdk-overview"

    # ----- Package settings and options -----
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "cmake"
    exports_sources = "BranchIO"

    # ----- Package dependencies -----
    # Pin to a specific version of Poco for stability of CI and other environments using
    # Conan. Conan recently introduced lockfiles, which may be useful for this purpose
    # as well. For now, this is consistent with transitive dependencies in Branch Maven
    # packages, to avoid version drift.
    requires = "Poco/1.10.1@pocoproject/stable"
    build_requires = "gtest/1.8.1@bincrafters/stable"

    def validate(self):
        if self.settings.os != "Windows":
            raise ConanInvalidConfiguration("Windows required")

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
            # build everything for now
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
        # that imports this one to test that we've been installed correctly, e.g. TestBed.
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
