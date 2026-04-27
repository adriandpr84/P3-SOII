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
#include "SearchTypes.hpp"


class PriorityScheduler {
public:
    void push(InternalSearchRequest petition, int queue_index);
    InternalSearchRequest pop();
    PriorityScheduler();

private:
    std::vector<std::queue<InternalSearchRequest>> petitions_queue;
};
