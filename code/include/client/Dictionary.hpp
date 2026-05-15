/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : Dictionary.hpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Clase para gestionar el diccionario de palabras de búsqueda.
 */

#pragma once

#include <string>
#include <vector>
#include <random>
#include <mutex>

class Dictionary {
private:
    std::vector<std::string> words;
    std::mt19937 rng; 
    std::mutex dict_mutex;

public:
    Dictionary();
    
    std::string get_random_word();
};