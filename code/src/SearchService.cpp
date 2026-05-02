/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchService.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : // TODO descripcion
 * Revision :
 * 27/05/2026 - Adrián - Creación de stubs para permitir compilación.
 */

#include "SearchService.hpp"
#include "SearchManager.hpp"
#include "ChunkSearcher.hpp"
#include <semaphore>

void SearchService::operator()() {

	auto &manager = SearchManager::get_instance();

	while (true) {
		InternalSearchRequest request = manager.get_search_petition();
		if (request.data.client_id == SHUTDOWN_REQUEST) {
			break;
		}

		std::vector<std::thread> chunk_threads;
		std::vector<BookResult> result;

		for (auto& book_metadata : library) {
			BookResult book_result = {};
			book_result.book_name = book_metadata.path;

			int thread_id = 0;
			int total_lines = book_metadata.line_index.size();
			int n_chunks = std::max(1, total_lines / MIN_CHUNK_SIZE);

			book_result.chunks.resize(n_chunks);

			int start_line = 1;
			for (int i = 0; i < n_chunks; i++) {
				int end_line = (i == n_chunks - 1) ? total_lines : start_line + MIN_CHUNK_SIZE - 1;
				int context_line_index = start_line - 2;
				std::streampos start_line_byte =
				    (i == 0) ? book_metadata.line_index[0] : book_metadata.line_index[context_line_index];

				chunks_resources.acquire();
				chunk_threads.emplace_back(ChunkSearcher(++thread_id, start_line, end_line, start_line_byte, book_metadata.path,
				                                   request.data.target_word, book_result.chunks[i], chunks_resources));
				
				start_line = end_line + 1;
			}
			result.push_back(book_result);
		}

		for (auto &chunk : chunk_threads) {
			chunk.join();
		}

		request.promise.set_value(std::move(result));
	}
}