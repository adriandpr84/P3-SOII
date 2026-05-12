/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Client.hpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Clase que modela un cliente como hilo independiente.
 */

#pragma once

#include <string>
#include "CommonTypes.hpp"

class Client {
private:
    int id;
    ClientType type;
    int balance;
    std::string target_word;

    void save_results_to_file(const SearchResult& result);

public:
    Client(int _id, ClientType _type, int _balance, std::string _target_word);
    
    void operator()();
};