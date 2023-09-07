from conans import ConanFile
# from conan.tools.cmake import CMakeToolchain
from conans import ConanFile, CMake, tools

class LoggerConan(ConanFile):
    name = "logger"
    version = "0.0.1"
    license = "MIT"
    url = "https://github.com/maratnek/loglib"  # Замените на URL вашего репозитория GitHub
    description = "Logger"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_find_package" 
    # , "cmake_find_package", "cmake_paths"
    exports_sources = "logger.hpp", "logger.cpp", "CMakeLists.txt"     

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    def requirements(self):
        self.requires("spdlog/1.12.0")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="src")  # Копируем заголовочные файлы
        self.copy("*.a", dst="lib", keep_path=False)

    # def generate(self):
    #     tc = CMakeToolchain(self)
    #     tc.generate()

    # def package(self):
        # cmake = CMake(self)
        # cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["logger"]