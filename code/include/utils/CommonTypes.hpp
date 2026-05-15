/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : CommonTypes.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Define tipos y estructuras que serán usados en todo el sistema
 */



#pragma once

#include "Occurrence.hpp"
#include "SearchResultsTypes.hpp"

enum class ClientType { FREE, PREMIUM_LIMITED, PREMIUM_UNLIMITED, COUNT };

struct SearchRequest {
    int client_id;
    ClientType type;
    std::string target_word;
    int balance;
};

