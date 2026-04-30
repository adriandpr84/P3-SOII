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
    // 1. Comprobaciones
    if (std::filesystem::is_directory(file_name)) {
        std::cerr << std::format("[ADVERTENCIA] Se omite '{}', es un directorio.\n", file_name);
        return;
    }

    if (is_binary(file_name)) {
        std::cerr << std::format("[ADVERTENCIA] Se omite '{}', no contiene texto plano.\n", file_name);
        return;
    }

    // 2. Preparación y ejecución
    std::vector<std::streampos> file_line_index;
    try{
        file_line_index = process_file(file_name);
    }catch(const std::exception& e){
        std::cerr << std::format("[ADVERTENCIA] Se omite '{}': {}\n", file_name, e.what());
        return;
    }
    
    create_searcher_threads(file_line_index);
    
}

// Crea los hilos, ejecuta la búsqueda y guarda el vector de Buscadores en el atributo 'resultados'
void BookSearcher::create_searcher_threads(std::vector<std::streampos> file_line_index) {
    int total_lines = file_line_index.size();
    if (thread_count > total_lines) {
        std::cerr << std::format(
            "[ADVERTENCIA] El archivo '{}' solo tiene {} líneas. Se ajustará el número de hilos a {}.\n", 
            file_name, total_lines, total_lines);
        thread_count = total_lines;
    }

    std::vector<std::thread> threads;
    
    chunks.reserve(thread_count);
    threads.reserve(thread_count);
    
    int read_chunk = total_lines / thread_count;
    int start_line = 1;

    std::mutex searcher_mutex;
    for (int thread_id = 1; thread_id <= thread_count; thread_id++) {
        int end_line = (thread_id == thread_count) ? total_lines : start_line + read_chunk - 1;
        int context_line_index = start_line - 2;
        std::streampos start_line_byte =
            (thread_id == 1) ? file_line_index[0] : file_line_index[context_line_index];
            
        chunks.emplace_back(thread_id, start_line, end_line, start_line_byte, file_name, target_word,
                                searcher_mutex);
        threads.emplace_back(std::ref(chunks.back()));
        start_line = end_line + 1;
    }

    for (auto &thread : threads) {
        thread.join();
    }
}


