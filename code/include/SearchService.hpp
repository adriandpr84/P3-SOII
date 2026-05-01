/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Archivo de cabecera de la clase SearchService. 
 *                Representa una replica de buscadores, procesará las peticiones de los clientes.
 */


#pragma once

#include "BookSearcher.hpp"
#include "SearchTypes.hpp"
#include <string>
#include <vector>

class SearchService {
  private:
    const std::vector<BookMetadata> library;
    int thread_count;

  public:
    SearchService(std::vector<BookMetadata> _library, int _thread_count)
        : library(_library), thread_count(_thread_count) {}
    void operator()();
};
