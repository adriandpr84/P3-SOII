/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ChunkSearcher.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 02/03/2026
 * Purpose      : Archivo de cabecera de la clase ChunkSearcher
 * Revision     :
 * 02/03/2026 - Adrián - Creación de la estructura de la clase ChunkSearcher según lo definido en clase 
 * Desde el 16/03/2026 hasta el 27/03/2026 se han ido añadiendo metodos necesarios a la clase 
 * según se desarrollaba funcionalidad y se refactorizaba en el ChunkSearcher.cpp
 */


#pragma once

#include "Occurrence.hpp"
#include "SearchResultsTypes.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <semaphore>

class ChunkSearcher {

  private:
    enum class Direction { BACKWARD = -1, FORWARD = 1 };

    // Variables
    int thread_id;
    int start_line;
    int end_line;
    std::streampos start_line_byte;
    std::string file_name;
    std::string target_word;
    std::vector<Occurrence> occurrences;
    ChunkResult& result;
    std::counting_semaphore<1024>& chunk_resources_sem;

    // Métodos
    void read_file();

    std::string prepare_initial_context(std::ifstream &file);
    void process_line(std::string &line, int line_num, const std::string &previous_context);
    void fetch_next_chunk_word(std::ifstream &file);

    void store_occurrence(const std::string &line_text, size_t match_pos, int line_num,
                              const std::string &last_word);
    void complete_pending_next_word(std::string &line_text);
    bool is_next_word_pending();

    std::string find_and_extract_word(const std::string &line, int pos, Direction dir);
    bool is_valid_match(int pos, std::string &target_word, std::string &line);
    bool is_separator(char c);
    bool is_in_range(int pos, const std::string &line);

  public:
    ChunkSearcher(int _thread_id, int _start, int _end, std::streampos _start_byte, std::string _file_name,
             std::string _target_word, ChunkResult& _result, std::counting_semaphore<1024>& _chunk_resources_sem)
        : thread_id(_thread_id), start_line(_start), end_line(_end), start_line_byte(_start_byte),
          file_name(_file_name), target_word(_target_word), result(_result), chunk_resources_sem(_chunk_resources_sem) {}
    void operator()();

    // Getters
    std::vector<Occurrence> get_occurrences() const { return occurrences; }
    int get_thread_id() const { return thread_id; }
    int get_start() const { return start_line; };
    int get_end() const { return end_line; };
    std::string get_target_word() const { return target_word; }
};
