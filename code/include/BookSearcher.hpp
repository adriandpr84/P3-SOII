/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : BookSearcher.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Archivo de cabecera de la clase BookSearcher
 * Revision     :
 * 18/03/2026 - Adrián - Creación de main minimo con procesamiento de argumentos y con un único hilo para pruebas. 
 * 20/03/2026 - Adrián - Creación completa de main. Incluye division de texto, creacion de hilos e impresión por pantalla. 
 * 24/03/2026 - Adrián - Control de edge cases en procesamiento de argumentos: Detección de binarios, validación de numero de hilos
 * 19/04/2026 - Adrián - Encapsulación del main de la P2 en la clase BookSearcher.
 */


#pragma once

#include "ChunkSearcher.hpp"
#include "SearchTypes.hpp"
#include "SearchResultsTypes.hpp"
#include <string>
#include <vector>

class BookSearcher {
  private:
    BookMetadata book_metadata;
    std::string target_word;
    int thread_count;
    std::vector<ChunkSearcher> chunks;
    BookResult& result;

    // Metodos privados
    void create_searcher_threads();

  public:
    BookSearcher(BookMetadata _metadata, std::string _target_word, BookResult& _result ,int _thread_count)
    : book_metadata(std::move(_metadata)), target_word(std::move(_target_word)), result(_result), thread_count(_thread_count) { }
    void operator()();
};
