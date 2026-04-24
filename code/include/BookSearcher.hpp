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


#ifndef BOOKSEARCHER_HPP
#define BOOKSEARCHER_HPP

#include "ChunkSearcher.hpp"
#include <string>
#include <vector>

class BookSearcher {
  private:
    std::string file_name;
    std::string target_word;
    int thread_count;
    std::vector<ChunkSearcher> chunks;

    // Metodos privados
    bool is_binary(const std::string &file_name);
    std::vector<std::streampos> process_file(const std::string &file_name);
    void create_searcher_threads(std::vector<std::streampos> file_line_index);

  public:
    BookSearcher::BookSearcher(std::string _file_name, std::string _target_word, int _thread_count)
    : file_name(std::move(_file_name)), target_word(std::move(_target_word)), thread_count(_thread_count) { }
    void operator()();
    std::vector<ChunkSearcher> get_results() const { return chunks; }
};

#endif