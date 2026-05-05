/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Client.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Modelar cada usuario como una entidad activa. Esta clase almacenará su ID, el tipo de cuenta (gratuita, premium, premium ilimitada) y su saldo si corresponde. 
 *                Ejecutará la petición bloqueándose a la espera de resultados y, al finalizar, calculará el tiempo total y guardará los resultados en un archivo privado.
 * * Revision : 
 * 
 */

 #include "Client.hpp"
#include "SearchManager.hpp"
#include <iostream>
#include <format>
#include <fstream>
#include <chrono>

Client::Client(int _id, ClientType _type, int _balance, std::string _target_word)
    : id(_id), type(_type), balance(_balance), target_word(std::move(_target_word)) {}

void Client::operator()() {
    std::string type_str = (type == ClientType::FREE) ? "GRATUITO" : (type == ClientType::PREMIUM_UNLIMITED) ? "PREMIUM ILIMITADO" : "PREMIUM LIMITADO";
                           
    std::cout << std::format("[CLIENTE {}] Creado. Tipo: {}. Palabra: '{}'.\n", id, type_str, target_word);

    // Empaquetar la petición.
    SearchRequest request;
    request.client_id = id;
    request.type = type;
    request.target_word = target_word;

    // Obtener la instancia del Singleton.
    SearchResult result = SearchManager::get_instance().execute_search(request);

    // Guardar resultados.
    save_results_to_file(result);

    std::cout << std::format("[CLIENTE {}] Finalizado. Resultados guardados.\n", id);
}

void Client::save_results_to_file(const SearchResult& result) {
    
}