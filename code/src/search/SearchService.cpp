/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Implementación de la división de tareas en hilos de búsqueda y consolidación de resultados.
 */

#include "search/SearchService.hpp"
#include "search/SearchManager.hpp"
#include "utils/ConcurrentQueue.hpp"

void SearchService::operator()() {

	auto &manager = SearchManager::get_instance();

	for (int i = 0; i < thread_count; i++) {
		chunk_threads.emplace_back(ChunkSearcher(task_queue));
	}

	int total_chunks = precalculate_chunks();

	while (true) {
		InternalSearchRequest request = manager.get_search_request();

		if (request.is_shutdown()) {
			stop_workers();
			break;
		}

		std::unique_ptr<SearchMonitor> monitor =
		    SearchMonitor::create(request.data.client_id, request.data.balance, request.data.type);

		std::vector<std::future<ChunkResult>> chunks_futures;
		chunks_futures.reserve(total_chunks);

		produce_tasks(request.data.target_word, chunks_futures, monitor.get());

		std::vector<BookResult> result = build_results(chunks_futures);

		request.promise.set_value(std::move(result));
	}
}

int SearchService::precalculate_chunks() {
	int total_chunks = 0;
	chunks_per_book.reserve(library.size());

	for (const auto &book : library) {
		int n_chunks = book.line_index.size();
		total_chunks += n_chunks;
		chunks_per_book.push_back(n_chunks);
	}

	return total_chunks;
}

void SearchService::produce_tasks(const std::string &target_word, std::vector<std::future<ChunkResult>> &chunks_futures,
                                  SearchMonitor* monitor) {

	for (auto &book_metadata : library) {
		int thread_id = 0;

		for (auto &chunk : book_metadata.line_index) {
			ChunkTask task(++thread_id, chunk.start_line, chunk.end_line, chunk.start_line_byte, book_metadata.path,
			               target_word, monitor);
			chunks_futures.push_back(task.get_future());
			task_queue.push(std::move(task));
		}
	}
}

std::vector<BookResult> SearchService::build_results(std::vector<std::future<ChunkResult>> &chunks_futures) {
    std::vector<BookResult> result;
    result.reserve(library.size());

    int future_idx = 0;

    for (size_t book_idx = 0; book_idx < library.size(); book_idx++) {
        BookResult book_result;
        book_result.book_name = library[book_idx].path;

        for (int n_chunk = 0; n_chunk < chunks_per_book[book_idx]; n_chunk++) {
            ChunkResult chunk_res = chunks_futures[future_idx++].get();

            if (!chunk_res.occurrences.empty() || !chunk_res.error_msg.empty()) {
                book_result.chunks.push_back(std::move(chunk_res));
            }
        }

        if (!book_result.chunks.empty()) {
            result.push_back(std::move(book_result));
        }
    }

    return result;
}

void SearchService::stop_workers() {
	if (chunk_threads.empty())
		return;

	int n_chunks_threads = static_cast<int>(chunk_threads.size());

	for (int i = 0; i < n_chunks_threads; i++)
		task_queue.push(ChunkTask::create_shutdown_task());

	for (auto &chunk : chunk_threads)
		if (chunk.joinable())
			chunk.join();

	chunk_threads.clear();
}

SearchService::~SearchService() { stop_workers(); }