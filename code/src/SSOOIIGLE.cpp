/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SSOOIIGLE.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano & Adrián Díaz-Plaza Remesal.
 * Purpose      : Esta clase realiza la simulación dividiendo el sistema en 2 procesos con fork().
 */

#include "search/SearchManager.hpp"
#include "client/ClientManager.hpp"
#include "payment/PaymentProxy.hpp"
#include "payment/PaymentProcess.hpp"
#include "utils/CommonTypes.hpp"
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>   

volatile sig_atomic_t g_shutdown_requested = 0;

void handle_sigint(int sig) {
    (void)sig;
    g_shutdown_requested = 1;

    std::signal(SIGINT, SIG_DFL);
}

int parse_arguments(int argc, char* argv[]) {
    if (argc > 1) {
        try { 
            return std::stoi(argv[1]); 
        } catch (...) { 
            std::cerr << "[ADVERTENCIA] Argumento inválido. Usando 50 clientes por defecto.\n";
        }
    }
    return 50; 
}

bool initialize_systems(const std::string& books_path) {
    try {
        PaymentProxy::get_instance().init();

        std::vector<PriorityEntry> priorities = {
            {80, {ClientType::PREMIUM_UNLIMITED, ClientType::PREMIUM_LIMITED}},
            {20, {ClientType::FREE}}
        };

        SearchManager::get_instance().init(books_path, priorities);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR CRÍTICO] Fallo en la inicialización: " << e.what() << "\n";
        return false;
    }
}

void shutdown_systems(pid_t child_pid) {
    std::cout << "\n[MAIN] Apagando el motor de búsqueda...\n";
    SearchManager::get_instance().shutdown();
    
    std::cout << "[MAIN] Enviando señal de apagado al sistema de pago...\n";
    PaymentProxy::get_instance().shutdown_remote_system();

    waitpid(child_pid, nullptr, 0);
}

int run_parent_process(pid_t child_pid, int num_clients, const std::string& books_path) {
    std::cout << std::format("[MAIN] Proceso principal (PID: {}). Hijo creado para pagos (PID: {}).\n", getpid(), child_pid);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (!initialize_systems(books_path)) {
        PaymentProxy::get_instance().shutdown_remote_system();
        waitpid(child_pid, nullptr, 0);
        return 1;
    }

    try {
        ClientManager client_manager(num_clients);

        client_manager.start_simulation("simulation_results", g_shutdown_requested);
        
        if (g_shutdown_requested) {
            std::cout << "\n[MAIN] Simulación finalizada prematuramente por el usuario.\n";
        } else {
            std::cout << "\n[MAIN] Todos los clientes han sido atendidos.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << std::format("[ERROR CRÍTICO] Simulación interrumpida: {}\n", e.what());
    }

    shutdown_systems(child_pid);
    return 0;
}

int main(int argc, char* argv[]) {

    std::signal(SIGINT, handle_sigint);

    int num_clients = parse_arguments(argc, argv);
    std::string books_path = "./libros"; 

    std::cout << "=== Iniciando Buscador SSOOIIGLE ===\n";
    std::cout << "Clientes a simular: " << num_clients << "\n\n";

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "[ERROR] Fallo al ejecutar fork().\n";
        return 1;
    } 
    else if (pid == 0) {

        std::signal(SIGINT, SIG_IGN); 
        PaymentProcess::run_server();
        return 0; 
    } 
    else {

        int exit_status = run_parent_process(pid, num_clients, books_path);
        
        std::cout << "=== Fin de la simulación ===\n";
        return exit_status;
    }
}