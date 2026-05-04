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

    // Aqui va la empaquetacion de la peticion

    // Despues obtendria la instancia 

    // Y ya guardo lo resultados.
}