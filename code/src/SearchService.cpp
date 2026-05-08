/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : // TODO descripcion
 * Revision :
 * 27/05/2026 - Adrián - Creación de stubs para permitir compilación.
 */

#include "SearchService.hpp"
#include "ConcurrentQueue.hpp"
#include "SearchManager.hpp"

void SearchService::operator()() {

	auto &manager = SearchManager::get_instance();

	for (int i = 0; i < thread_count; i++) {
		chunk_threads.emplace_back(ChunkSearcher(task_queue));
	}

	while (true) {
		InternalSearchRequest request = manager.get_search_request();
		if (request.is_shutdown()) {
			stop_workers();
			break;
		}

		std::vector<int> chunks_per_book;
		int total_chunks = precalculate_chunks(chunks_per_book);

		std::vector<std::future<ChunkResult>> chunks_futures;
		chunks_futures.reserve(total_chunks);

		produce_tasks(request.data.target_word, chunks_per_book, chunks_futures);

		std::vector<BookResult> result = build_results(chunks_per_book, chunks_futures);

		request.promise.set_value(std::move(result));
	}
}

int SearchService::precalculate_chunks(std::vector<int> &chunks_per_book) {
	int total_chunks = 0;
	chunks_per_book.reserve(library.size());

	for (const auto &book : library) {
		int lines = book.line_index.size();
		int n_chunks = std::max(1, lines / MIN_CHUNK_SIZE);
		total_chunks += n_chunks;
		chunks_per_book.push_back(n_chunks);
	}

	return total_chunks;
}

void SearchService::produce_tasks(const std::string &target_word, const std::vector<int> &chunks_per_book,
                                  std::vector<std::future<ChunkResult>> &chunks_futures) {
	int book_idx = 0;
	for (auto &book_metadata : library) {
		int thread_id = 0;
		int total_lines = book_metadata.line_index.size();
		int n_chunks = chunks_per_book[book_idx++];

		int start_line = 1;
		for (int i = 0; i < n_chunks; i++) {
			int end_line = (i == n_chunks - 1) ? total_lines : start_line + MIN_CHUNK_SIZE - 1;
			int context_line_index = start_line - 2;
			std::streampos start_line_byte =
			    (i == 0) ? book_metadata.line_index[0] : book_metadata.line_index[context_line_index];

			ChunkTask task(++thread_id, start_line, end_line, start_line_byte, book_metadata.path, target_word);
			chunks_futures.push_back(task.get_future());
			task_queue.push(std::move(task));
			start_line = end_line + 1;
		}
	}
}

std::vector<BookResult> SearchService::build_results(const std::vector<int> &chunks_per_book,
                                                     std::vector<std::future<ChunkResult>> &chunks_futures) {
	std::vector<BookResult> result;
	result.reserve(library.size());

	int future_idx = 0;

	for (size_t book_idx = 0; book_idx < library.size(); book_idx++) {
		BookResult book_result;
		book_result.chunks.reserve(chunks_per_book[book_idx]);
		result.push_back(std::move(book_result));
		result.back().book_name = library[book_idx].path;

		for (int n_chunk = 0; n_chunk < chunks_per_book[book_idx]; n_chunk++) {
			result.back().chunks.push_back(chunks_futures[future_idx++].get());
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