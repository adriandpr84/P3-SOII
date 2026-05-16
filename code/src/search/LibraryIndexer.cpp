/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : LibraryIndexer.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 30/04/2026
 * Purpose      : Implementación del indexado de archivos mediante el cálculo de offsets de línea.
 */

#include "search/LibraryIndexer.hpp"
#include "utils/SystemConfig.hpp"
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::vector<BookMetadata> LibraryIndexer::prepare_library(const std::string &directory_path) {

	std::vector<BookMetadata> library;

	if (!std::filesystem::exists(directory_path)) {
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

		std::vector<ChunkMetadata> index = process_file(file_path);
		if (index.empty()) {
			std::cerr << std::format("[WARNING] Omitiendo el archivo: {}. No se pudo abrir/procesar. \n", file_path);
			continue;
		}

		library.emplace_back(file_path, std::move(index));
	}

	return library;
}

// Escanea el archivo e indexa la posición de inicio (byte) de cada línea
std::vector<ChunkMetadata> LibraryIndexer::process_file(const std::string &file_path) {
	std::vector<ChunkMetadata> file_line_index;
	std::ifstream file(file_path);

	if (!file)
		return file_line_index;

	int current_line = 1;
	file_line_index.push_back({current_line, 0, 0}); // Index primera linea

	std::string line_text;

	while (std::getline(file, line_text)) {
		current_line++;

		if (current_line % Config::LINES_PER_CHUNK == 0) {
			std::streampos position = file.tellg();

			if (position != std::streampos(-1) && file.peek() != EOF) {
				file_line_index.back().end_line = current_line;
				file_line_index.push_back({current_line, 0, position});
			}
		}
	}

	file_line_index.back().last_chunk = true;
	file_line_index.back().end_line = current_line - 1;

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