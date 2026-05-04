/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Client.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Modelar cada usuario como una entidad activa. Esta clase almacenará su ID, el tipo de cuenta (gratuita, premium, premium ilimitada) y su saldo si corresponde. 
 *                Ejecutará la petición bloqueándose a la espera de resultados y, al finalizar, calculará el tiempo total y guardará los resultados en un archivo privado.
 * * Revision : 
 * 
 */


#include "Dictionary.hpp"

Dictionary::Dictionary() {
    
    std::random_device rd;
    rng = std::mt19937(rd());

    words = {
        "sistemas", "operativos", "concurrencia", "hilos", 
        "procesos", "memoria", "mutex", "semaforo", "computador", "red"
    };
}

std::string Dictionary::get_random_word() {
    std::lock_guard<std::mutex> lock(dict_mutex);
    if (words.empty()) return "";

    std::uniform_int_distribution<int> dist(0, words.size() - 1);
    return words[dist(rng)];
}