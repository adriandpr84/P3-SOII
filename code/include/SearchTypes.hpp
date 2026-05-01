/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchTypes.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Define las estructuras y tipos que usará internamente el motor de busqueda.
 * Revision     :
 * 
 */

#pragma once

#include "CommonTypes.hpp"
#include <future>

struct InternalSearchRequest {
    SearchRequest data;
    std::promise<std::vector<BookResult>> promise{};

    std::future<std::vector<BookResult>> get_future() {
        return promise.get_future();
    }
};
struct BookMetadata {
    std::string path;
    std::vector<std::streampos> line_index;
};


