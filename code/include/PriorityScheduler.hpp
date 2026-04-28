/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : PriorityScheduler.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Implementa el planificador de prioridades de clientes, 
 *                gestionando las colas según proporción.
 */


#pragma once

#include <string>
#include <vector>
#include <queue>
#include <random>
#include <mutex>
#include "SearchTypes.hpp"

constexpr int MAX_STARVATION = 100;

struct ManagedQueue {
    std::queue<InternalSearchRequest> requests_queue;
    int starvation_counter = 0;
};

class PriorityScheduler {
public:
    PriorityScheduler(std::vector<int> priority_weights);

    void push(InternalSearchRequest petition, int queue_index);
    InternalSearchRequest pop();

private:

    std::mutex scheduler_mutex;
    std::condition_variable cv_empty_queue;

    std::mt19937 random_gen;
    std::discrete_distribution<int> priority_selector;

    std::vector<ManagedQueue> priority_queues;
    int total_pending_requests = 0;


    int determine_queue_extraction();
    InternalSearchRequest extract_from_queue(ManagedQueue& target_queue);
    void update_starvation(int served_idx);
};
