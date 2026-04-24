/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchResults.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Definicion de estructuras para almacenar los resultados, 
 *                para desacoplar los resultados de la funcionalidad de los hilos.
 */


#ifndef SEARCHRESULTSTYPES_HPP
#define SEARCHRESULTSTYPES_HPP

#include <string>
#include <vector>
#include "Occurrence.hpp"

struct ChunkResult {
    int thread_id;
    int start_line;
    int end_line;
    std::vector<Occurrence> occurrences;
};

struct BookResult {
    std::string book_name;
    std::vector<ChunkResult> chunks;
};

struct SearchResult {
    double total_time;
    std::vector<BookResult> results_per_book;
};


#endif

