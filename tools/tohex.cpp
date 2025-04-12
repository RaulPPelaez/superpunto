#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

std::string to_variable_name(const std::string& path) {
    std::string name = path;
    size_t last_slash = name.find_last_of("/\\");
    if (last_slash != std::string::npos) {
	name = name.substr(last_slash + 1);
    }
    // prepend "shareds_"
    name = "shaders_" + name;

    for (auto& c : name) {
        if (!std::isalnum(c)) c = '_';
    }
    return name;
}

void write_shader_as_array(std::ofstream& out, const std::string& filepath) {
    std::ifstream shader_file(filepath, std::ios::binary);
    if (!shader_file) {
        std::cerr << "Failed to open " << filepath << "\n";
        return;
    }

    std::ostringstream array;
    std::string var_name = to_variable_name(filepath);

    array << "const char " << var_name << "[] = {\n    ";
    int byte_count = 0;
    char byte;
    while (shader_file.get(byte)) {
        array << "0x" << std::hex << std::setw(2) << std::setfill('0')
              << (static_cast<unsigned>(static_cast<unsigned char>(byte))) << ", ";
        if (++byte_count % 12 == 0) {
            array << "\n    ";
        }
    }
    // Add null byte at end
    array << "0x00\n};\n\n";

    out << array.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file1> <file2> ...\n";
        return 1;
    }

    std::ofstream header("shaders.h");
    if (!header) {
        std::cerr << "Failed to open shaders.h for writing\n";
        return 1;
    }

    header << "#ifndef SHADERS_H\n#define SHADERS_H\n\n";
    for (int i = 1; i < argc; ++i) {
        std::string filename = argv[i];
        if (!filename.empty()) {
            write_shader_as_array(header, filename);
        }
    }
    header << "\n#endif\n";
    std::cout << "shaders.h generated from " << argc - 1 << " file(s).\n";
    return 0;
}
