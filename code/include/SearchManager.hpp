/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchManager.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Actúa como nexo de unión entre cliente y motor de búsqueda. 
 * Abstrae toda la complejidad interna del motor y proporciona
 * el servicio de búsqueda a los clientes.
 */

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include "CommonTypes.hpp"       // SearchResult y SearchRequest
#include "SearchTypes.hpp"       // InternalSearchRequest
#include "SearchService.hpp"
#include "PriorityScheduler.hpp"

struct PriorityEntry {
    int priority;
    std::vector<ClientType> client_types;
};

class SearchManager {
public:
    static SearchManager& get_instance() {
        static SearchManager instance;
        return instance;
    }

    ~SearchManager();

    void init(std::string path_books);
    void shutdown();

    SearchResult execute_search(SearchRequest request);

    InternalSearchRequest get_search_request();

    // Prevención de copias y asignaciones
    SearchManager(const SearchManager&) = delete;
    SearchManager& operator=(const SearchManager&) = delete;

private:

    std::unordered_map<ClientType, int> client_queue_map;
    std::vector<std::thread> searchers_threads;
    std::vector<std::unique_ptr<SearchService>> searchers;
    std::unique_ptr<PriorityScheduler> scheduler_ptr;
    std::atomic<bool> is_running = false;
    int idx_shutdown_queue;

    std::vector<int> build_and_validate_queues(const std::vector<PriorityEntry>& config);

    SearchManager();
};
