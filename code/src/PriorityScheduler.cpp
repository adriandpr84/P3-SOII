/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : PriorityScheduler.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : // TODO descripcion
 * Revision :
 * 27/05/2026 - Adrián - Creación de stubs para permitir compilación.
 */


#include "PriorityScheduler.hpp"

PriorityScheduler::PriorityScheduler(std::vector<int> weights)
    : random_gen(std::random_device{}()), 
      priority_selector(weights.begin(), weights.end()), 
      priority_queues(weights.size()+1) {}

   
void PriorityScheduler::push(InternalSearchRequest request, int queue_index) {
    std::lock_guard<std::mutex> lock(scheduler_mutex);
    
    priority_queues[queue_index].requests_queue.push(std::move(request));
    total_pending_requests++;
    
    cv_empty_queue.notify_one();
}

InternalSearchRequest PriorityScheduler::pop() {
    std::unique_lock<std::mutex> lock(scheduler_mutex);
    cv_empty_queue.wait(lock, [this] { return total_pending_requests > 0; });

    int served_idx = determine_queue_extraction();
    InternalSearchRequest request = extract_from_queue(priority_queues[served_idx]);
    update_starvation(served_idx);

    return request;
}


int PriorityScheduler::determine_queue_extraction() {

	int n_queues = static_cast<int>(priority_queues.size()-1);

	//Extracción por starvation
    for (int i = 0; i < n_queues; ++i) {
        if (priority_queues[i].starvation_counter >= MAX_STARVATION && 
            !priority_queues[i].requests_queue.empty()) {
            return i;
        }
    }

	//Extracción por prioridad
    int start_idx = priority_selector(random_gen);
    for (int i = 0; i < n_queues; ++i) {
        int current_idx = (start_idx + i) % priority_queues.size();
        if (!priority_queues[current_idx].requests_queue.empty()) {
            return current_idx;
        }
    }

    return priority_queues.size() - 1; // Si se llega aqui, DEBE ser la cola de finalización
}

InternalSearchRequest PriorityScheduler::extract_from_queue(ManagedQueue& target_queue) {
    InternalSearchRequest request = std::move(target_queue.requests_queue.front());
    target_queue.requests_queue.pop();
    
    target_queue.starvation_counter = 0;
    total_pending_requests--;               
    
    return request;
}

void PriorityScheduler::update_starvation(int served_idx) {
	int n_queues = static_cast<int>(priority_queues.size());
    for (int i = 0; i < n_queues; ++i) {
        if (i != served_idx && !priority_queues[i].requests_queue.empty()) {
            priority_queues[i].starvation_counter++;
        }
    }
}
