/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Representa una réplica del buscador que gestiona un pool de hilos para procesar una petición completa.
 */

#pragma once

#include "ChunkSearcher.hpp"
#include "payment/SearchMonitor.hpp"
#include "utils/SearchResultsTypes.hpp"
#include "utils/SearchTypes.hpp"
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

class SearchService {
  private:
	int replica_id;
	const std::vector<BookMetadata> library;
	int thread_count;
	std::vector<int> chunks_per_book;
	std::vector<std::thread> chunk_threads;
	ConcurrentQueue<ChunkTask> task_queue;

	void produce_tasks(const std::string &target_word, std::vector<std::future<ChunkResult>> &chunks_futures,
	                   SearchMonitor* monitor);

	int precalculate_chunks();

	std::vector<BookResult> build_results(std::vector<std::future<ChunkResult>> &chunks_futures);

	void stop_workers();

  public:
	SearchService(int _replica_id, std::vector<BookMetadata> _library, int _thread_count)
	    : replica_id(_replica_id), library(std::move(_library)), thread_count(_thread_count) {}
	~SearchService();

	void operator()();
};