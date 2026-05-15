/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchManager.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Orquestación de servicios de búsqueda, inicialización de réplicas y gestión de apagado.
 */

#include "search/SearchManager.hpp"
#include "search/LibraryIndexer.hpp"
#include <format>
#include <unordered_set>

constexpr int FACTOR = 2;
constexpr int N_SEARCH_SERVICE = 4;
constexpr int MAX_ALLOWED_THREADS = 1000;

SearchManager::SearchManager() {}

std::vector<int> SearchManager::build_and_validate_queues(const std::vector<PriorityEntry> &config) {
	std::vector<int> extracted_weights;
	int actual_queue_index = 0;
	int total_weight = 0;
	std::unordered_set<ClientType> types_proccessed;

	for (const auto &[priority, client_types] : config) {
		extracted_weights.push_back(priority);
		total_weight += priority;

		for (const auto &client : client_types) {
			client_queue_map[static_cast<size_t>(client)] = actual_queue_index;
			if (!types_proccessed.emplace(client).second) {
				throw std::invalid_argument("[SEARCH MANAGER] Error: ClientType duplicado.");
			}
		}
		actual_queue_index++;
	}

	if (total_weight != 100) {
		throw std::invalid_argument("[SEARCH MANAGER] Error: Las prioridades deben sumar 100.");
	}

	if (types_proccessed.size() != static_cast<size_t>(ClientType::COUNT)) {
		throw std::invalid_argument("[SEARCH MANAGER] Error: No se han configurado todos los tipos de cliente.");
	}

	return extracted_weights;
}

// ** Inicializacion del motor de busqueda, debe ser invocado en el hilo principal **
void SearchManager::init(std::string path_books, const std::vector<PriorityEntry> &priority_config) {

	std::unique_lock<std::shared_mutex> lock(manager_mtx);
	if (is_running)
		return;

	// Iniciar scheduler
	std::vector<int> extracted_weights = build_and_validate_queues(priority_config);
	idx_shutdown_queue = extracted_weights.size();
	scheduler_ptr = std::make_unique<PriorityScheduler>(extracted_weights);

	// Indexar libros
	LibraryIndexer library_indexer;
	std::vector<BookMetadata> library;
	try {
		library = library_indexer.prepare_library(path_books);
	} catch (const std::invalid_argument &e) {
		throw std::runtime_error(std::format("[SEARCH MANAGER] Error al indexar los libros: {}", e.what()));
	}

	// Lanzar workers
	unsigned int cores = std::thread::hardware_concurrency() * FACTOR;
	unsigned int base_threads = (cores != 0) ? cores : N_SEARCH_SERVICE;
	base_threads = std::min(base_threads, (unsigned int)MAX_ALLOWED_THREADS);
	int n_threads_per_replica = std::max(1u, base_threads / N_SEARCH_SERVICE);

	searchers_threads.reserve(N_SEARCH_SERVICE);
	searchers.reserve(N_SEARCH_SERVICE);

	for (int i = 0; i < N_SEARCH_SERVICE; i++) {
		searchers.push_back(std::make_unique<SearchService>(library, n_threads_per_replica));
		searchers_threads.emplace_back(std::ref(*searchers.back()));
	}

	is_running = true;
}

// ** El cliente debe invocar este método **
SearchResult SearchManager::execute_search(SearchRequest request) { // Bloqueante

	if (request.target_word.empty()) {
		throw std::invalid_argument("[SEARCH MANAGER] Error: La palabra a buscar no puede estar vacía.");
	}

	if (request.client_id < 0) {
		throw std::invalid_argument("[SEARCH MANAGER] Error: El ID de cliente no puede ser negativo.");
	}

	auto start_time = std::chrono::high_resolution_clock::now();

	InternalSearchRequest internal_request(request);
	std::future request_future = internal_request.get_future();
	int queue_index = client_queue_map[static_cast<size_t>(request.type)];

	{
		std::shared_lock<std::shared_mutex> lock(manager_mtx);
		if (!is_running) {
			throw std::runtime_error("[SEARCH MANAGER] El motor de busqueda no acepta peticiones.");
		}
		scheduler_ptr->push(std::move(internal_request), queue_index);
	}

	std::vector<BookResult> result = request_future.get();

	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
	return SearchResult{elapsed.count(), result};
}

InternalSearchRequest SearchManager::get_search_request() { // Bloqueante

	return scheduler_ptr->pop();
}

void SearchManager::shutdown() {

	std::unique_lock<std::shared_mutex> lock(manager_mtx);
	if (!is_running)
		return;
	is_running = false;

	int n_searchers = static_cast<int>(searchers_threads.size());

	for (int i = 0; i < n_searchers; i++)
		scheduler_ptr->push(InternalSearchRequest::create_shutdown_request(), idx_shutdown_queue);

	for (auto &searcher : searchers_threads)
		if (searcher.joinable())
			searcher.join();

	searchers_threads.clear();
	searchers.clear();
}

SearchManager::~SearchManager() { shutdown(); }
