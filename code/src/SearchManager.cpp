/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchManager.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : // TODO descripcion
 * Revision : 
 * 27/05/2026 - Adrián - Creación de stubs para permitir compilación.
 */

/*
    Esta clase NO es un hilo, no contiene void operator()(). 
    Gestiona el acceso al monitor PriorityScheduler y atiende a los clientes.
    al metodo execute_search entran hilos cliente y al get_search_request entran hilos SearchService.
*/   

#include "SearchManager.hpp"

#define PATH_BOOKS

SearchManager::SearchManager() {
    /// --- Definición de prioridades --- ///
    std::vector<std::pair<int, std::vector<ClientType>>> config_queues = {
        { 80, { ClientType::PREMIUM_LIMITED, ClientType::PREMIUM_UNLIMITED } },
        { 20, { ClientType::FREE } }
    };
    /// --------------------------------- ///
    /*
    // TODO: Implementar validación de config_queues  
        - Comprobar que todas las keys sumen 100
        - Comprobar que no falta ningun tipo  
    */

    std::vector<int> extracted_weights;
    int actual_queue_index=0;
    for (const auto& [priority, client_types] : config_queues) {
        extracted_weights.push_back(priority);
        for (const auto& client : client_types){
            client_queue_map.emplace(client, actual_queue_index);
        }
        actual_queue_index++;

    }
    this->scheduler_ptr = std::make_unique<PriorityScheduler>(extracted_weights);
}


// ** Inicializacion del motor de busqueda, debe ser invocado en el hilo principal **
void SearchManager::init(){
    // TODO: Implementar lógica
    /*
    - Se encargará de inicializar todo el sistema de buscadores.
    - Recorre directorios y hace las comprobaciones de libros. Genera un std::vector con la lista de libros a procesar.
    - Lanza hilos SearchService
    */
}


// ** El cliente debe invocar este método **
SearchResult SearchManager::execute_search(SearchRequest request) { // Bloqueante

    std::promise<std::vector<BookResult>> request_promise;
    std::future<std::vector<BookResult>> request_future = request_promise.get_future();

    InternalSearchRequest internal_request{request, std::move(request_promise)};

    int queue_index = client_queue_map[request.type];
    scheduler_ptr->push(std::move(internal_request), queue_index);

    std::vector<BookResult> result = request_future.get();
    //TODO: Medir tiempo
    return SearchResult{0.0, result};
}

InternalSearchRequest SearchManager::get_search_petition() { //Bloqueante

    return scheduler_ptr->pop();
}

SearchManager::~SearchManager(){ 
    //TODO: Implementar lógica
    /*
    - Es posible que se deba gestionar la finalizacion de hilos inferiores (SearchService). 
    - Se debe controlar que rompan su bucle y hacer su posterior join().
    - Valorar más adelante si se deberian interrumpir todas las busquedas en curso o esperar a que finalicen. 
    */
}


