/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Client.hpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Clase que modela un cliente como hilo independiente.
 */

#pragma once

#include <string>
#include <vector>
#include "SearchManager.hpp"

enum class ClientType {
    FREE,
    PREMIUM_LIMITED,
    PREMIUM_UNLIMITED
};

class Client {
private:
    int id;
    ClientType type;
    int balance;
    std::string target_word;

    void save_results_to_file(const SearchResult& result, double elapsed_seconds);

public:
    Client(int id, ClientType type, int balance, std::string target_word, SearchManager& sm);
    
    void operator()();
};