/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ClientManager.hpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Generador de clientes en tandas a lo largo del tiempo.
 */

#pragma once

#include "SearchManager.hpp"
#include "Dictionary.hpp"
#include <vector>
#include <thread>

class ClientManager {
private:
    Dictionary dict;
    int total_clients;
    std::vector<std::thread> client_threads;

    ClientType generate_random_type();

public:
    ClientManager(int total_clients);
    ~ClientManager();

    void start_simulation();
};