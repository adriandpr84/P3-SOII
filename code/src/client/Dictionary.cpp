/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Dictionary.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Gestionar las palabras que se pueden buscar. Al crear el usuario se escoge una palabra al azar.
 *                Puede inicializarse leyendo un archivo o con palabras hardcodeadas.
 */


#include "client/Dictionary.hpp"

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