/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchMonitor.hpp
 * Purpose      : Interfaz base para los monitores de búsqueda y su Factoría.
 */

#pragma once

#include "utils/CommonTypes.hpp"
#include <memory>

class SearchMonitor {
public:
    virtual ~SearchMonitor() = default;

    virtual bool should_stop() = 0;
    virtual void wait_for_payment() = 0;
    virtual bool register_match() = 0;

    static std::unique_ptr<SearchMonitor> create(int client_id, int initial_balance, ClientType type);
};