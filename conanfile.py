from conans import ConanFile
from conans import ConanFile, CMake, tools

class LoggerConan(ConanFile):
    name = "logger"
    version = "0.2.2"
    license = "MIT"
    url = "https://github.com/maratnek/loglib"  # Замените на URL вашего репозитория GitHub
    package_type = "library"
    description = "Logger"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_find_package" 
    exports_sources = "include/*", "src/*", "CMakeLists.txt"     

    # Binary configuration
    # options = {"shared": [True, False], "build_type": ["Debug", "Release"]}
    # default_options = {"shared": False, "build_type": "Debug"}

    def build_id(self):
        self.info_build.settings.build_type = "Any"

    def requirements(self):
        self.requires("spdlog/1.12.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake_release = CMake(self, build_type="Release")
        cmake_release.configure()
        cmake_release.build()

        cmake_debug = CMake(self, build_type="Debug")
        cmake_debug.configure()
        cmake_debug.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["logger"]