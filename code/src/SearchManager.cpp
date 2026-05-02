/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchManager.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : // TODO descripcion
 * Revision :
 * 27/05/2026 - Adrián - Creación de stubs para permitir compilación.
 */

#include "SearchManager.hpp"
#include "LibraryIndexer.hpp"
#include <format>
#include <unordered_set>

#define DIRECTORY_PATH_BOOKS "libros"
constexpr int N_SEARCH_SERVICE = 4;
constexpr int MAX_ALLOWED_THREADS = 1024; // NO mas de 1024, cada thread abre un archivo. El número maximo de descriptores por proceso es 1024.

SearchManager::SearchManager() {

	/// --- Definición de prioridades --- ///
	std::vector<PriorityEntry> priority_config = {{80, {ClientType::PREMIUM_LIMITED, ClientType::PREMIUM_UNLIMITED}},
	                                              {20, {ClientType::FREE}}};
	/// --------------------------------- ///

	std::vector<int> extracted_weights = build_and_validate_queues(priority_config);

	idx_shutdown_queue = extracted_weights.size();

	scheduler_ptr = std::make_unique<PriorityScheduler>(extracted_weights);
}

std::vector<int> SearchManager::build_and_validate_queues(const std::vector<PriorityEntry> &config) {
	std::vector<int> extracted_weights;
	int actual_queue_index = 0;
	int total_weight = 0;
	std::unordered_set<ClientType> types_proccessed;

	for (const auto &[priority, client_types] : config) {
		extracted_weights.push_back(priority);
		total_weight += priority;

		for (const auto &client : client_types) {
			client_queue_map.emplace(client, actual_queue_index);
			if (!types_proccessed.emplace(client).second) {
				throw std::runtime_error("[SearchManager] Error: ClientType duplicado.");
			}
		}
		actual_queue_index++;
	}

	if (total_weight != 100) {
		throw std::runtime_error("[SearchManager] Error: Las prioridades deben sumar 100.");
	}

	if (types_proccessed.size() != static_cast<size_t>(ClientType::COUNT)) {
		throw std::runtime_error("[SearchManager] Error: No se han configurado todos los tipos de cliente.");
	}

	return extracted_weights;
}

// ** Inicializacion del motor de busqueda, debe ser invocado en el hilo principal **
void SearchManager::init() {
	if (is_running)
		return;

	LibraryIndexer library_indexer;
	std::vector<BookMetadata> library = library_indexer.prepare_library(DIRECTORY_PATH_BOOKS);

	unsigned int cores = std::thread::hardware_concurrency();
	unsigned int base_threads = (cores != 0) ? cores : N_SEARCH_SERVICE;
	base_threads = std::min(base_threads, (unsigned int)MAX_ALLOWED_THREADS);
	int n_threads_per_replica = std::max(1u, base_threads / N_SEARCH_SERVICE);

	searchers.reserve(N_SEARCH_SERVICE);

	for (int i = 0; i < N_SEARCH_SERVICE; i++) {
		searchers.emplace_back(SearchService(library, n_threads_per_replica));
	}

	is_running = true;
}

// ** El cliente debe invocar este método **
SearchResult SearchManager::execute_search(SearchRequest request) { // Bloqueante
	if (!is_running) {
		throw std::runtime_error("[SEARCHMANAGER] El motor de busqueda no acepta peticiones");
	}

	InternalSearchRequest internal_request(request);
	std::future request_future = internal_request.get_future();

	int queue_index = client_queue_map.at(request.type);
	scheduler_ptr->push(std::move(internal_request), queue_index);

	std::vector<BookResult> result = request_future.get();
	// TODO: Medir tiempo
	return SearchResult{0.0, result};
}

InternalSearchRequest SearchManager::get_search_petition() { // Bloqueante

	return scheduler_ptr->pop();
}

SearchManager::~SearchManager() {
	is_running = false;
	int n_searchers = static_cast<int>(searchers.size());

	for (int i = 0; i < n_searchers; i++) {
		InternalSearchRequest shudown_request;
		scheduler_ptr->push(std::move(shudown_request), idx_shutdown_queue);
	}

	for (auto &searcher : searchers) {
		searcher.join();
	}
}
