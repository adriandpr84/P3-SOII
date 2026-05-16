/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ClientManager.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Encargarse de la creación de los clientes. Controlará la instanciación de hilos.
 */

#include "client/ClientManager.hpp"
#include "client/Client.hpp"
#include "utils/SystemConfig.hpp"
#include <filesystem>
#include <format>
#include <iostream>
#include <chrono>

ClientManager::ClientManager(int _total_clients) : total_clients(_total_clients) {}

ClientManager::~ClientManager() {
    wait_for_clients();
}
// Método para esperar a que todos los hilos cliente finalicen antes de destruir el ClientManager.
void ClientManager::wait_for_clients() {
    for (auto &t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}
// Método para generar un tipo de cliente aleatorio basado en las probabilidades definidas en SystemConfig.
ClientType ClientManager::generate_random_type() {
    int random_val = rand() % 100;
    if (random_val < Config::PROB_FREE) {
        return ClientType::FREE;
    } else if (random_val < (Config::PROB_FREE + Config::PROB_PREMIUM_LIMITED)) {
        return ClientType::PREMIUM_LIMITED;
    } else {
        return ClientType::PREMIUM_UNLIMITED;
    }
}
// Método para preparar el directorio de resultados.
void ClientManager::prepare_directory(const std::string &path) {
    if (std::filesystem::exists(path)) {
        std::filesystem::remove_all(path);
    }
    std::filesystem::create_directories(path);
}
// Método principal para iniciar la simulación de clientes.
void ClientManager::start_simulation(const std::string &simulation_path, const volatile sig_atomic_t& shutdown_flag) {
    std::cout << std::format("[CLIENT MANAGER] Iniciando simulación con {} clientes...\n", total_clients);
    try {
        prepare_directory(simulation_path);
    } catch (const std::filesystem::filesystem_error &e) {
        throw std::runtime_error(std::format("[ERROR] No se pudo crear el directorio de resultados"));
    }
    
    int clients_created = 0;
    // Lanzar clientes en lotes para simular la llegada gradual de usuarios.
    while (clients_created < total_clients && !shutdown_flag) {
        int clients_to_create = std::min(Config::CLIENT_BATCH_SIZE, total_clients - clients_created);

        for (int i = 0; i < clients_to_create; i++) {
            int current_id = clients_created + 1;
            ClientType type = generate_random_type();

            int balance = (type == ClientType::PREMIUM_LIMITED) ? (rand() % Config::RECHARGE_AMOUNT + Config::MIN_INITIAL_BALANCE) : 0;
            std::string word = dict.get_random_word();

            Client new_client(current_id, type, balance, word, simulation_path);
            client_threads.emplace_back(std::thread(new_client));

            clients_created++;
        }

        if (clients_created < total_clients && !shutdown_flag) {
            std::this_thread::sleep_for(std::chrono::milliseconds(Config::BATCH_INTERVAL_MS));
        }
    }

    if (shutdown_flag) {
        std::cout << "\n[CLIENT MANAGER] Interrupción detectada. Esperando a que finalicen las búsquedas en curso...\n";
    } else {
        std::cout << "\n[CLIENT MANAGER] Todos los clientes han sido lanzados. Esperando finalizaciones...\n";
    }
    
    wait_for_clients();
}