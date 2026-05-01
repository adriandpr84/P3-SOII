/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : LibraryIndexer.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 30/04/2026
 * Purpose      : //TODO descripcion
 * Revision     :
 *
 */



#pragma once 

#include "SearchTypes.hpp"
#include <vector>
#include <string>
#include <filesystem>
#include <utility> 


class LibraryIndexer {
public:
    std::vector<BookMetadata> prepare_library(const std::string& directory_path);

private:
    bool is_binary(const std::string& file_name);
    std::vector<std::streampos> process_file(const std::string& file_name);
};