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
Este monitor estará protegido con un mutex y una CV para dormir a los hilos SearchService en caso de que no haya nada en
ninguna cola.

*/

#include "PriorityScheduler.hpp"

PriorityScheduler::PriorityScheduler(std::vector<int> weights)
    : gen(std::random_device{}()), dist(weights.begin(), weights.end()), request_queues(weights.size()) {}

void PriorityScheduler::push(InternalSearchRequest request, int queue_index) {

	std::lock_guard<std::mutex> lock(queue_mutex);
    {
        request_queues[queue_index].requests.push(std::move(request));
	    global_request_count++;
    }
    cv_empty_queue.notify_one();
}

InternalSearchRequest PriorityScheduler::pop() {

	std::unique_lock<std::mutex> lock(queue_mutex);
	cv_empty_queue.wait(lock, [this] { return global_request_count > 0; });
	int n_queues = request_queues.size();
	int start_idx = dist(gen);

	for (int i = 0; i < n_queues; ++i) {
		int current_idx = (start_idx + i) % n_queues;

		if (!request_queues[current_idx].requests.empty()) {
			InternalSearchRequest request = std::move(request_queues[current_idx].requests.front());
			request_queues[current_idx].requests.pop();
			global_request_count--;

			return request;
		}
	}
    
    return {}; // Nunca se llegaría aquí
}

/*
    *Posiblemente se pueda incorporar en un futuro aging de colas para evitar inanición,
    sin embargo, habria que valorar si eso afectaria de forma notable a la proporcion global a largo plazo.

*/
