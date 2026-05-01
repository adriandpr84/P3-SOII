/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : BookSearcher.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Clase que procesa un libro entero, divide el texto, crea hilos buscadores y almacena los resultados.
 * * Revision : 
 * 18/03/2026 - Adrián - Creación de main minimo con procesamiento de argumentos y con un único hilo para pruebas. 
 * 20/03/2026 - Adrián - Creación completa de main. Incluye division de texto, creacion de hilos e impresión por pantalla. 
 * 24/03/2026 - Adrián - Control de edge cases en procesamiento de argumentos: Detección de binarios, validación de numero de hilos
 * 19/04/2026 - Adrián - Encapsulación del main de la P2 en la clase BookSearcher.
 */

#include "BookSearcher.hpp"
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <filesystem>
#include <stdexcept>

void BookSearcher::operator()() {    
    create_searcher_threads();
}

// Crea los hilos, ejecuta la búsqueda y guarda el vector de Buscadores en el atributo 'resultados'
void BookSearcher::create_searcher_threads() {
    std::vector<ChunkResult> chunk_results;
    chunk_results.resize(thread_count);

    int total_lines = book_metadata.line_index.size();

    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    
    int read_chunk = total_lines / thread_count;
    int start_line = 1;

    for (int thread_id = 1; thread_id <= thread_count; thread_id++) {
        int end_line = (thread_id == thread_count) ? total_lines : start_line + read_chunk - 1;
        int context_line_index = start_line - 2;
        std::streampos start_line_byte =
            (thread_id == 1) ? book_metadata.line_index[0] : book_metadata.line_index[context_line_index];

        threads.emplace_back(ChunkSearcher(thread_id, start_line, end_line, start_line_byte, book_metadata.path, target_word, chunk_results[thread_id-1]));
        start_line = end_line + 1;
    }

    for (auto &thread : threads) {
        thread.join();
    }
    
    result.book_name = book_metadata.path;
    result.chunks = std::move(chunk_results);
}


