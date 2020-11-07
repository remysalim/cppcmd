import os
from conans import ConanFile, CMake, tools


class CppCmdConan(ConanFile):
    name = "cppcmd"
    version = "0.1.0"
    settings = "os", "compiler", "arch", "build_type"
    url = "https://github.com/remysalim/cppcmd"
    license = "MIT"
    description = "Simple cpp command interpreter header-only library"
    topics = ("header-only", "interpreter", "cpp")
    no_copy_source = True
    exports_sources = "include/*"
    generators = "cmake_find_package"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }
    options = {"build_tests": [True, False]}
    default_options = ("build_tests=False")

    def build(self):
        cmake = CMake(self)
        cmake.definitions["BUILD_TESTING"] = self.options.build_tests
        cmake.configure()
        cmake.build()
        if not tools.cross_building(self) and self.options.build_tests:
            cmake.test()
        cmake.install()

    def build_requirements(self):
        if self.options.build_tests:
            self.build_requires("catch2/2.13.3")

    def package_id(self):
        self.info.header_only()
