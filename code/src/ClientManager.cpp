/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ClientManager.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Encargarse de la creación de los clientes. Controlará la instanciación de hilos y su posterior finalización
 * 
 */

#include "ClientManager.hpp"
#include "Client.hpp"
#include <iostream>
#include <format>
#include <chrono>

ClientManager::ClientManager(int _total_clients) 
    : total_clients(_total_clients) {}

ClientManager::~ClientManager() {
    
    for (auto& t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

ClientType ClientManager::generate_random_type() {
    
    // Proporción aproximada: 20% Gratis, 80% Premium (repartido entre limitado e ilimitado)
    // Lo voy ha hacer con una estructura de if-else.
}