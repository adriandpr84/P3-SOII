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

 

SearchManager::SearchManager() {}


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
    // TODO: Implementar lógica
    /*
    - Invocado por el cliente, introduce su peticion
    - Esa peticion se envuelve en una peticion interna que contendrá promise
    - Se determina a que cola debe ir
    - Se llama a priorityscheduler.enqueue()
    - Espera el resultado con future.get_value()
    - return del resultado al cliente
    */
    return SearchResult{};
}

InternalSearchRequest SearchManager::get_search_petition() { //Bloqueante
    // TODO: Implementar lógica
    /*
    - Invocado por SearchService.
        priorityscheduler.pop()
    */
    return InternalSearchRequest{}; 
}

SearchManager::~SearchManager(){ 
    //TODO: Implementar lógica
    /*
    - Es posible que se deba gestionar la finalizacion de hilos inferiores (SearchService). 
    - Se debe controlar que rompan su bucle y hacer su posterior join().
    - Valorar más adelante si se deberian interrumpir todas las busquedas en curso o esperar a que finalicen. 
    */
}


