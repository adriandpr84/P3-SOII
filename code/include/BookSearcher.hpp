/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : BookSearcher.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Archivo de cabecera de la clase BookSearcher
 */


#ifndef BOOKSEARCHER_HPP
#define BOOKSEARCHER_HPP

#include "Searcher.hpp"
#include <string>
#include <vector>

class BookSearcher {
  private:
    std::string file_name;
    std::string target_word;
    int thread_count;
    std::vector<Searcher> results;

    // Metodos privados
    bool is_binary(const std::string &file_name);
    std::vector<std::streampos> process_file(const std::string &file_name);
    void create_searcher_threads(std::vector<std::streampos> file_line_index);

  public:
    BookSearcher(std::string _file_name, std::string _target_word, int _thread_count);
    void operator()();
    std::vector<Searcher> get_results() const { return results; }
};

#endif