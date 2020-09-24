import os
from conans import ConanFile, CMake, tools


class CppShellConan(ConanFile):
    name = "cppshell"
    version = "0.1.0"
    settings = "os", "compiler", "arch", "build_type"
    url = "http://echosens-tfs:8080/tfs/EchoCollection/eSW/_git/cppshell"
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

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if not tools.cross_building(self):
            cmake.test()

    def build_requirements(self):
        self.build_requires("catch2/2.13.1")

    def package(self):
        self.copy("*.hpp")

    def package_id(self):
        self.info.header_only()
