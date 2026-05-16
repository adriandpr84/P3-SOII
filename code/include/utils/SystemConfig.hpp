/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SystemConfig.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Define las constantes utilizadas por todo el sistema.
 */


#pragma once
#include <string>

namespace Config {
    // -- Búsqueda y Rendimiento --
    constexpr int LINES_PER_CHUNK = 2000;
    constexpr int FACTOR = 2;
    constexpr int MAX_ALLOWED_THREADS = 1000; // NO subir mas. El max de descriptores en Linux es 1024

    // -- Reglas del Buscador --
    constexpr int MAX_WORDS_FREE = 20;
    constexpr int MAX_STARVATION = 100;
    
    // -- Reglas Económicas (Pagos) --
    constexpr int RECHARGE_AMOUNT = 10;       
    constexpr int MIN_INITIAL_BALANCE = 5;    
    constexpr int MAX_INITIAL_BALANCE = 14;   

    // -- Configuración de la Simulación (ClientManager) --
    constexpr int CLIENT_BATCH_SIZE = 5;      
    constexpr int BATCH_INTERVAL_MS = 1000;
    constexpr int PROXY_INIT_DELAY_MS = 200;
    const std::string PATH_RESULTS = "simulation_results";
    
    // -- Probabilidades de Generación de Clientes (%) --
    constexpr int PROB_FREE = 40;
    constexpr int PROB_PREMIUM_LIMITED = 40;  
    /* El resto será PREMIUM_UNLIMITED */
}