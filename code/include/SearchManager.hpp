/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchManager.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Actúa como nexo de unión entre cliente y motor de búsqueda. 
 *                Abstrae toda la complejidad interna del motor y proporciona
 *                el servicio de búsqueda a los usuarios.
 */


#ifndef SEARCH_MANAGER_HPP
#define SEARCH_MANAGER_HPP

#include <string>
#include <vector>
#include <thread>
#include "CommonTypes.hpp"
#include "SearchTypes.hpp"
#include "SearchService.hpp"
#include "PriorityScheduler.hpp"


class SearchManager {
public:
    
    static SearchManager& get_instance() {
        static SearchManager instance;
        return instance;
    }

    void execute_search();
    InternalSearchRequest get_search_petition();

    // Prevención de copias 
    SearchManager(const SearchManager&) = delete;
    SearchManager& operator=(const SearchManager&) = delete;

private:
    std::vector<std::thread> searchers;
    PriorityScheduler scheduler;
    SearchManager();
};

#endif 