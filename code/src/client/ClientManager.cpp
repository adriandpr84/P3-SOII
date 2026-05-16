/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ClientManager.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Encargarse de la creación de los clientes. Controlará la instanciación de hilos y su posterior
 * finalización
 */

#include "client/ClientManager.hpp"
#include "client/Client.hpp"
#include <filesystem>
#include <format>
#include <iostream>

ClientManager::ClientManager(int _total_clients) : total_clients(_total_clients) {}

ClientManager::~ClientManager() {
    wait_for_clients();
}

void ClientManager::wait_for_clients() {
    for (auto &t : client_threads) {
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

void ClientManager::prepare_directory(const std::string &path) {
    if (std::filesystem::exists(path)) {
        std::filesystem::remove_all(path);
    }
    std::filesystem::create_directories(path);
}

void ClientManager::start_simulation(const std::string &simulation_path, const volatile sig_atomic_t& shutdown_flag) {
    std::cout << std::format("[CLIENT MANAGER] Iniciando simulación con {} clientes...\n", total_clients);
    try {
        prepare_directory(simulation_path);
    } catch (const std::filesystem::filesystem_error &e) {
        throw std::runtime_error(std::format("[ERROR] No se pudo crear el directorio de resultados"));
    }
    
    int clients_created = 0;
    int batch_size = 5;

    while (clients_created < total_clients && !shutdown_flag) {
        int clients_to_create = std::min(batch_size, total_clients - clients_created);

        for (int i = 0; i < clients_to_create; i++) {
            int current_id = clients_created + 1;
            ClientType type = generate_random_type();

            int balance = (type == ClientType::PREMIUM_LIMITED) ? (rand() % 10 + 5) : 0;
            std::string word = dict.get_random_word();

            Client new_client(current_id, type, balance, word);
            client_threads.emplace_back(std::thread(new_client));

            clients_created++;
        }

        if (clients_created < total_clients && !shutdown_flag) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    if (shutdown_flag) {
        std::cout << "\n[CLIENT MANAGER] Interrupción detectada. Esperando a que finalicen las búsquedas en curso...\n";
    } else {
        std::cout << "\n[CLIENT MANAGER] Todos los clientes han sido lanzados. Esperando finalizaciones...\n";
    }
    
    wait_for_clients();
}