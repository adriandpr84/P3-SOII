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
#include "utils/SystemConfig.hpp"
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

struct SimulationConfig {
    int num_clients = 50;
    int num_replicas = 4;
    std::string books_path = "./libros"; 
};

SimulationConfig parse_arguments(int argc, char* argv[]) {
    SimulationConfig config;
    
    if (argc > 1) {
        try { config.num_clients = std::stoi(argv[1]); } 
        catch (...) { std::cerr << "[AVISO] Fallo al leer clientes. Usando 50 por defecto.\n"; }
    }
    
    if (argc > 2) {
        try { config.num_replicas = std::stoi(argv[2]); } 
        catch (...) { std::cerr << "[AVISO] Fallo al leer réplicas. Usando 4 por defecto.\n"; }
    }
    
    if (argc > 3) {
        config.books_path = argv[3]; 
    }
    
    if (argc > 4){
        throw std::invalid_argument(std::format("Uso: {} clientes replicas ruta_libros\n", argv[0]));
    }
    return config;
}

bool initialize_systems(const SimulationConfig& config) {
    try {
        PaymentProxy::get_instance().init();

        std::vector<PriorityEntry> priorities = {
            {80, {ClientType::PREMIUM_UNLIMITED, ClientType::PREMIUM_LIMITED}},
            {20, {ClientType::FREE}}
        };

        SearchManager::get_instance().init(config.books_path, priorities, config.num_replicas);
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

int run_parent_process(pid_t child_pid, const SimulationConfig& config) {
    std::cout << std::format("[MAIN] Proceso principal (PID: {}). Hijo creado para pagos (PID: {}).\n", getpid(), child_pid);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if (!initialize_systems(config)) {
        PaymentProxy::get_instance().shutdown_remote_system();
        waitpid(child_pid, nullptr, 0);
        return 1;
    }

    try {
        ClientManager client_manager(config.num_clients);

        client_manager.start_simulation(Config::PATH_RESULTS, g_shutdown_requested);
        
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
    SimulationConfig config;

    try{
        config = parse_arguments(argc, argv);
    }catch (const std::exception& e) {
        std::cerr << std::format("[ERROR] {}\n", e.what());
    }

    std::cout << "=== Iniciando Buscador SSOOIIGLE ===\n";
    std::cout << std::format("Clientes: {} | Réplicas: {} | Directorio: {}\n\n", 
                             config.num_clients, config.num_replicas, config.books_path);

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
        int exit_status = run_parent_process(pid, config);
        
        std::cout << "=== Fin de la simulación ===\n";
        std::cout << std::format("Resultados disponibles en: {}\n", Config::PATH_RESULTS);
        return exit_status;
    }
}