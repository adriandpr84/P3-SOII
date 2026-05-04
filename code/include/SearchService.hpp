/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Archivo de cabecera de la clase SearchService. 
 * Representa una replica de buscadores, procesará las peticiones de los clientes.
 */

#pragma once

#include "SearchTypes.hpp"
#include "ChunkSearcher.hpp" // Necesario para ChunkTask y ChunkResult
#include <string>
#include <vector>
#include <semaphore>
#include <mutex>
#include <future>
#include <queue>
#include <condition_variable>

constexpr int MIN_CHUNK_SIZE = 2000;

class SearchService {
  private:
    const std::vector<BookMetadata> library;
    int thread_count;
    std::vector<std::thread> chunk_threads;
    ConcurrentQueue<ChunkTask> task_queue;

    int precalculate_chunks(std::vector<int> &chunks_per_book);
    
    void produce_tasks(const std::string &target_word, 
                       const std::vector<int> &chunks_per_book,
                       std::vector<std::future<ChunkResult>> &chunks_futures);
                       
    std::vector<BookResult> build_results(const std::vector<int> &chunks_per_book,
                                          std::vector<std::future<ChunkResult>> &chunks_futures);

  public:
    SearchService(std::vector<BookMetadata> _library, int _thread_count)
        : library(std::move(_library)), thread_count(_thread_count) {}
    ~SearchService();
        
    void operator()();
};