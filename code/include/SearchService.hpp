/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Archivo de cabecera de la clase SearchService. 
 *                Representa una replica de buscadores, procesará las peticiones de los clientes.
 */


#pragma once

#include "SearchTypes.hpp"
#include <string>
#include <vector>
#include <semaphore>
#include <mutex>

constexpr int MIN_CHUNK_SIZE = 2000;
class SearchService {
  private:
    const std::vector<BookMetadata> library;
    int thread_count;
    std::counting_semaphore<1024> chunks_resources;

  public:
    SearchService(std::vector<BookMetadata> _library, int _thread_count)
        : library(std::move(_library)), thread_count(_thread_count), chunks_resources(_thread_count) {}
    void operator()();
};
