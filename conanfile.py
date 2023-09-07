from conans import ConanFile, CMake, tools

class LoggerConan(ConanFile):
    name = "logger"
    version = "1.0.0"
    license = "MIT"
    url = "https://github.com/yourusername/logger"  # Замените на URL вашего репозитория GitHub
    description = "Logger"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "logger.hpp", "logger.cpp"     

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="src")  # Копируем заголовочные файлы
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["logger"]
