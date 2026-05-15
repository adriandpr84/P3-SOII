/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchManager.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Punto de entrada principal del motor de búsqueda, abstrae la complejidad interna del
 * sistema a los clientes.
 */

#pragma once

#include "CommonTypes.hpp" // SearchResult y SearchRequest
#include "PriorityScheduler.hpp"
#include "SearchService.hpp"
#include "SearchTypes.hpp" // InternalSearchRequest
#include <array>
#include <atomic>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

struct PriorityEntry {
	int priority;
	std::vector<ClientType> client_types;
};

class SearchManager {
  public:
	static SearchManager &get_instance() {
		static SearchManager instance;
		return instance;
	}

	~SearchManager();

	void init(std::string path_books, const std::vector<PriorityEntry> &priority_config);
	void shutdown();

	SearchResult execute_search(SearchRequest request);

	InternalSearchRequest get_search_request();

	// Prevención de copias y asignaciones
	SearchManager(const SearchManager &) = delete;
	SearchManager &operator=(const SearchManager &) = delete;

  private:
	std::array<int, static_cast<size_t>(ClientType::COUNT)> client_queue_map;
	std::vector<std::thread> searchers_threads;
	std::vector<std::unique_ptr<SearchService>> searchers;
	std::unique_ptr<PriorityScheduler> scheduler_ptr;
	bool is_running = false;
	std::shared_mutex manager_mtx;
	int idx_shutdown_queue;

	std::vector<int> build_and_validate_queues(const std::vector<PriorityEntry> &config);

	SearchManager();
};
