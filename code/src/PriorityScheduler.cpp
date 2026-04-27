/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : PriorityScheduler.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : // TODO descripcion
 * Revision : 
 * 27/05/2026 - Adrián - Creación de stubs para permitir compilación.
 */

/*

Se encargará de alamcenar las colas de peticiones, será usado por SearchManager (que contiene la logica de negocio)
La unica funcion de esta clase es almacenar las peticiones y sacarlas segun prioridad
Este monitor estará protegido con un mutex y una CV para dormir a los hilos SearchService en caso de que no haya nada en ninguna cola.

*/

#include "PriorityScheduler.hpp"

// Constructor vacío
PriorityScheduler::PriorityScheduler() {}

void PriorityScheduler::push(InternalSearchRequest request, int queue_index) {
    // TODO: Implementar lógica
/*
Mete las peticiones en la cola que corresponda.
*/
    
}

InternalSearchRequest PriorityScheduler::pop() {
    // TODO: Implementar lógica

    /*
    Saca de alguna de las colas una peticion.
    funcionamiento:
        - genera un numero aleatorio del 1 al 100
        - se compara ese numero con las proporciones q tenemos (ya sea 80/20, 90/5/5, podria cambiar)
        - segun al rango que entre, se extrae una peticion de una cola u otra
        - si esa cola está vacia, fallback a la posterior
        *Posiblemente se pueda incorporar en un futuro aging de colas para evitar inanición, 
        sin embargo, habria que valorar si eso afectaria de forma notable a la proporcion global a largo plazo.

    */

    return InternalSearchRequest{};
}