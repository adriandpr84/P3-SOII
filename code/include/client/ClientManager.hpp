/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ClientManager.hpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Generador de clientes en tandas a lo largo del tiempo.
 */

#pragma once

#include "utils/CommonTypes.hpp"
#include "Dictionary.hpp"
#include <vector>
#include <thread>

class ClientManager {
private:
    Dictionary dict;
    int total_clients;
    std::vector<std::thread> client_threads;

    void prepare_directory(const std::string& path);
    ClientType generate_random_type();
    void wait_for_clients();

public:
    ClientManager(int total_clients);
    ~ClientManager();

    void start_simulation(const std::string &simulation_path);
};