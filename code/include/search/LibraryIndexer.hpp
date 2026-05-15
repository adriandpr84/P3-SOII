/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : LibraryIndexer.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 30/04/2026
 * Purpose      : Escanea el directorio de libros, descarta archivos binarios y precalcula los
 * chunks para los hilos.
 */

#pragma once

#include "utils/SearchTypes.hpp"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class LibraryIndexer {
  public:
	std::vector<BookMetadata> prepare_library(const std::string &directory_path);

  private:
	bool is_binary(const std::string &file_name);
	std::vector<ChunkMetadata> process_file(const std::string &file_name);
};