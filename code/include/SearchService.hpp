/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Archivo de cabecera de la clase SearchService. 
 *                Representa una replica de buscadores, procesará las peticiones de los clientes.
 */


#ifndef SEARCHSERVICE_HPP
#define SEARCHSERVICE_HPP

#include "BookSearcher.hpp"
#include <string>
#include <vector>

class SearchService {
  private:
    const std::vector<std::string> book_paths;
    int thread_count;

  public:
    SearchService(std::vector<std::string> _book_paths, int _thread_count)
        : book_paths(_book_paths), thread_count(_thread_count) {}
    void operator()();
};

#endif