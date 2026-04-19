/**
 * Project      : Practica 2 de Sistemas Operativos II
 * Program      : LibroBuscador.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Archivo de cabecera de la clase ServicioBuscador (Orquestador multihilo)
 */


#ifndef LIBROBUSCADOR_HPP
#define LIBROBUSCADOR_HPP

#include "Buscador.hpp"
#include <string>
#include <vector>

class LibroBuscador {
  private:
    std::string nombre_archivo;
    std::string palabra_buscada;
    int n_hilos;
    std::vector<Buscador> resultados;

    // Métodos auxiliares ahora encapsulados
    bool es_binario(const std::string &nombre_archivo);
    std::vector<std::streampos> procesar_archivo(const std::string &nombre_archivo);
    void crear_hilos_buscadores(std::vector<std::streampos> indice_lineas_archivo);

  public:
    
    LibroBuscador(std::string _nombre_archivo, std::string _palabra_buscada, int _n_hilos);
    void operator()();
    std::vector<Buscador> get_resultados() const { return resultados; }
};

#endif