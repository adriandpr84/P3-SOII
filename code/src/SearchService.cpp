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

void SearchService::operator()() {
	auto &manager = SearchManager::get_instance();

	while (true) {

		InternalSearchRequest request = manager.get_search_petition();
		if (request.data.client_id == SHUTDOWN_REQUEST){
			break;
		}

		std::vector<BookResult> results_per_book;
		results_per_book.resize(library.size());

		std::vector<std::thread> book_searchers;
		book_searchers.reserve(library.size());

		for (int i = 0; i < library.size(); i++)
			book_searchers.emplace_back(BookSearcher(library[i], request.data.target_word, results_per_book[i], 0));

		for (auto &searcher : book_searchers)
			searcher.join();

		request.promise.set_value(std::move(results_per_book));
	}
}
