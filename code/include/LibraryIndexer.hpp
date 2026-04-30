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