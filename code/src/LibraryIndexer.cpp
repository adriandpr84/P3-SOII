#include "LibraryIndexer.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <format>

std::vector<BookMetadata> LibraryIndexer::prepare_library(const std::string &directory_path) {

    std::vector<BookMetadata> library;

    if (!std::filesystem::exists(directory_path)){
        throw std::invalid_argument("[ERROR] No existe el directorio.");
    }
    
    for (const auto &entry : std::filesystem::directory_iterator(directory_path)) {
        if (!entry.is_regular_file())
            continue;

        std::string file_path = entry.path().string();

        if (is_binary(file_path)) {
            std::cerr << std::format("[INFO] Omitiendo archivo binario: {}\n", file_path);
            continue;
        }

        std::vector<std::streampos> index = process_file(file_path);
        if (index.empty()){
            std::cerr << std::format("[WARNING] Omitiendo el archivo: {}. No se pudo abrir/procesar. \n", file_path);
            continue;
        }

        library.emplace_back(file_path, std::move(index));
    }

    return library;
}

// Escanea el archivo e indexa la posición de inicio (byte) de cada línea
std::vector<std::streampos> LibraryIndexer::process_file(const std::string &file_path) {
    std::vector<std::streampos> file_line_index;
    std::ifstream file(file_path);

    if (!file)
        return file_line_index;

    file_line_index.push_back(0); // index primera linea
    std::string line_text;
    while (std::getline(file, line_text)) {
        std::streampos position = file.tellg();
        if (position != -1) {
            file_line_index.push_back(position);
        }
    }
    return file_line_index;
}

// Detecta si un archivo es binario buscando bytes nulos (\0) en los primeros
// 8000 bytes, misma heurística que usa Git.
// Ref: https://github.com/git/git/blob/master/xdiff-interface
constexpr int FIRST_BYTES = 8000;
bool LibraryIndexer::is_binary(const std::string &file_path) {
    std::ifstream f(file_path, std::ios::binary);
    char buffer[FIRST_BYTES];
    std::streamsize read_count = f.read(buffer, sizeof(buffer)).gcount();
    return std::memchr(buffer, '\0', read_count) != nullptr;
}