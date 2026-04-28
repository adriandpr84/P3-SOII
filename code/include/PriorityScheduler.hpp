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

#define MAX_AGE 1000

struct QueueSlot {
    std::queue<InternalSearchRequest> requests;
    int age = 0;
};


class PriorityScheduler {
public:
    void push(InternalSearchRequest petition, int queue_index);
    InternalSearchRequest pop();
    PriorityScheduler(std::vector<int> weights);

private:
    std::mutex queue_mutex;
    std::condition_variable cv_empty_queue;

    std::mt19937 gen;
    std::discrete_distribution<int> dist;
    std::vector<int> weights;

    std::vector<QueueSlot> request_queues;
    int global_request_count = 0;

};
