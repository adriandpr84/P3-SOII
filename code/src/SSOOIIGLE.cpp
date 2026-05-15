/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SSOOIIGLE.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Esta clase realiza la simulación dividiendo el sistema en 2 procesos con fork().
 */

#include "SearchManager.hpp"
#include "ClientManager.hpp"
#include "PaymentProxy.hpp"
#include "PaymentProcess.hpp"
#include "CommonTypes.hpp"
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int num_clients = 50; 
    if (argc > 1) {
        try { num_clients = std::stoi(argv[1]); } 
        catch (...) { 
            std::cerr << "[ADVERTENCIA] Argumento inválido. Usando 50 clientes por defecto.\n";
            num_clients = 50; 
        }
    }

    std::string books_path = "./libros"; 

    std::cout << "=== Iniciando Buscador SSOOIIGLE ===\n";
    std::cout << "Clientes a simular: " << num_clients << "\n\n";

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "[ERROR] Fallo al ejecutar fork().\n";
        return 1;
    } 
    else if (pid == 0) {

        PaymentProcess::run_server();
        
        return 0; 
    } 
    else {

        std::cout << std::format("[MAIN] Proceso principal (PID: {}). Hijo creado para pagos (PID: {}).\n", getpid(), pid);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        try {
            PaymentProxy::get_instance().init();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
            waitpid(pid, nullptr, 0);
            return 1;
        }

        std::vector<PriorityEntry> priorities = {
            {80, {ClientType::PREMIUM_UNLIMITED, ClientType::PREMIUM_LIMITED}},
            {20, {ClientType::FREE}}
        };

        try {
            SearchManager::get_instance().init(books_path, priorities);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR CRÍTICO] " << e.what() << "\n";
            PaymentProxy::get_instance().shutdown_remote_system();
            waitpid(pid, nullptr, 0);
            return 1;
        }

        ClientManager client_manager(num_clients);

        client_manager.start_simulation("simulation_results");

        std::cout << "\n[MAIN] Todos los clientes han sido atendidos.\n";
        std::cout << "[MAIN] Apagando el motor de búsqueda...\n";
        SearchManager::get_instance().shutdown();
        
        std::cout << "[MAIN] Enviando señal de apagado al sistema de pago...\n";
        PaymentProxy::get_instance().shutdown_remote_system();

        waitpid(pid, nullptr, 0);
        
        std::cout << "=== Fin de la simulación ===\n";
        return 0;
    }
}