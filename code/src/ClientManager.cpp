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
    
    int random_val = rand() % 100;
    if (random_val < 20) {
        return ClientType::FREE;
    } else if (random_val < 60) {
        return ClientType::PREMIUM_LIMITED;
    } else {
        return ClientType::PREMIUM_UNLIMITED;
    }
}

void ClientManager::start_simulation() {

    // Crear clientes en tandas

    // Asignar los saldos iniciales dependiendo del tipo de cliente

    // Inciar los hilos de los clientes

    // Hacer pausa para simular la llegada de clientes 
}