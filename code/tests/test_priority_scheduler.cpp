#include <iostream>
#include <utility>
#include <thread>
#include <vector>
#include <atomic>
#include <string>
#include "search/PriorityScheduler.hpp"

// Función auxiliar para generar peticiones
InternalSearchRequest create_dummy_request(int client_id) {
    SearchRequest req;
    req.client_id = client_id;
    InternalSearchRequest internal_req(req);
    return internal_req; 
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " [FIFO|FALLBACK|RATIO|CONCURRENCY|STARVATION]\n";
        return 1;
    }

    std::string test_type = argv[1];

    if (test_type == "FIFO") {
        PriorityScheduler scheduler({80, 20});
        scheduler.push(create_dummy_request(101), 0);
        scheduler.push(create_dummy_request(102), 0);
        if (scheduler.pop().data.client_id != 101) return 1;
        if (scheduler.pop().data.client_id != 102) return 1;
    } 
    
    else if (test_type == "FALLBACK") {
        PriorityScheduler scheduler({100, 0});
        scheduler.push(create_dummy_request(999), 1); // Forzar uso de la cola sin prioridad

        if (scheduler.pop().data.client_id != 999) {
            return 1;
        }
    } 
    
    else if (test_type == "RATIO") {
        PriorityScheduler scheduler({80, 20});
        for (int i = 0; i < 99999; i++) {
            scheduler.push(create_dummy_request(1), 0);
            scheduler.push(create_dummy_request(2), 1);
        }
        int premium = 0;
        for (int i = 0; i < 1000; i++) {
            if (scheduler.pop().data.client_id == 1) premium++;
        }
        double ratio = static_cast<double>(premium) / 1000;

        if (std::abs(ratio - 0.80) > 0.05) {
            return 1;
        }
        
    } 
    
    else if (test_type == "CONCURRENCY") {
        PriorityScheduler scheduler({80, 20});
        const int num_prod = 100, num_cons = 4, reqs_x_prod = 1000;
        std::atomic<int> procesadas{0};
        std::vector<std::thread> hilos;

        // Iniciar consumidores
        for (int i = 0; i < num_cons; ++i) {
            hilos.push_back(std::thread([&]() {
                while (true) {
                    auto req = scheduler.pop();
                    if (req.data.client_id == -1) break;
                    procesadas++;
                }
            }));
        }

        // Generar peticiones
        for (int i = 0; i < num_prod; ++i) {
            hilos.push_back(std::thread([&, i]() {
                for (int j = 0; j < reqs_x_prod; ++j) scheduler.push(create_dummy_request(i), i % 2);
            }));
        }
        for (int i = 0; i < num_prod; ++i) hilos[num_cons + i].join();

        // Finalizar (mandando shutdown request a la cola de shutdown)
        for (int i = 0; i < num_cons; ++i) scheduler.push(create_dummy_request(-1), 2);
        for (int i = 0; i < num_cons; ++i) hilos[i].join();

        // Comprobación
        int total_esperadas = num_prod * reqs_x_prod;
        if (procesadas != total_esperadas) {
            return 1;
        }
        
    }

    else if (test_type == "STARVATION") {
        PriorityScheduler scheduler({100, 0});
        const int MAX_AGE = 100; 

        scheduler.push(create_dummy_request(1), 1);

        for (int i = 0; i <= MAX_AGE; i++) {
            scheduler.push(create_dummy_request(0), 0);
        }

        for (int i = 0; i < MAX_AGE; i++) {
            if (scheduler.pop().data.client_id != 0) {
                std::cerr << "La petición salió antes de tiempo.\n";
                return 1; 
            }
        }

        if (scheduler.pop().data.client_id != 1) {
            std::cerr << "La petición no ha salido (hay starvation)\n";
            return 1;
        }
    }

    return 0;
}