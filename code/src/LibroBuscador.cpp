/**
 * Project      : Practica 2 de Sistemas Operativos II
 * Program      : LibroBuscador.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Purpose      : Clase que procesa un libro entero, divide el texto, crea hilos buscadores y almacena los resultados.
 * * Revision : 
 * 18/03/2026 - Adrián - Creación de main minimo con procesamiento de argumentos y con un único hilo para pruebas. 
 * 20/03/2026 - Adrián - Creación completa de main. Incluye division de texto, creacion de hilos e impresión por pantalla. 
 * 24/03/2026 - Adrián - Control de edge cases en procesamiento de argumentos: Detección de binarios, validación de numero de hilos
 */

#include "../include/LibroBuscador.hpp"
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <filesystem>
#include <stdexcept>

LibroBuscador::LibroBuscador(std::string _nombre_archivo, std::string _palabra_buscada, int _n_hilos)
    : nombre_archivo(std::move(_nombre_archivo)), palabra_buscada(std::move(_palabra_buscada)), n_hilos(_n_hilos) { }

void LibroBuscador::operator()() {
    // 1. Comprobaciones
    if (std::filesystem::is_directory(nombre_archivo)) {
        std::cerr << std::format("Advertencia: Se omite '{}', es un directorio.\n", nombre_archivo);
        exit(EXIT_FAILURE);
    }

    if (es_binario(nombre_archivo)) {
        std::cerr << std::format("Advertencia: Se omite '{}', no contiene texto plano.", nombre_archivo);
        exit(EXIT_FAILURE);
    }

    // 2. Preparación y ejecución
    std::vector<std::streampos> indice_lineas_archivo = procesar_archivo(nombre_archivo);
    crear_hilos_buscadores(indice_lineas_archivo);
}


// Crea los hilos, ejecuta la búsqueda y guarda el vector de Buscadores en el atributo 'resultados'
void LibroBuscador::crear_hilos_buscadores(std::vector<std::streampos> indice_lineas_archivo) {
    int total_lineas = indice_lineas_archivo.size();
    if (n_hilos > total_lineas) {
        std::cerr << std::format(
            "Advertencia: el archivo '{}' solo tiene {} líneas. Se ajustará el número de hilos a {}.\n", 
            nombre_archivo, total_lineas, total_lineas);
        n_hilos = total_lineas;
    }

    std::vector<std::thread> hilos;
    
    resultados.reserve(n_hilos);
    hilos.reserve(n_hilos);
    
    int porcion_lectura = total_lineas / n_hilos;
    int linea_inicio = 1;

    std::mutex mutex_buscadores;
    for (int id_hilo = 1; id_hilo <= n_hilos; id_hilo++) {
        int linea_fin = (id_hilo == n_hilos) ? total_lineas : linea_inicio + porcion_lectura - 1;
        int indice_linea_contexto = linea_inicio - 2;
        std::streampos linea_byte_inicio =
            (id_hilo == 1) ? indice_lineas_archivo[0] : indice_lineas_archivo[indice_linea_contexto];
            
        // Usamos emplace_back directamente en el atributo de la clase
        resultados.emplace_back(id_hilo, linea_inicio, linea_fin, linea_byte_inicio, nombre_archivo, palabra_buscada,
                                mutex_buscadores);
        hilos.emplace_back(std::ref(resultados.back()));
        linea_inicio = linea_fin + 1;
    }

    for (auto &hilo : hilos) {
        hilo.join();
    }
}

// Escanea el archivo e indexa la posición de inicio (byte) de cada línea 
std::vector<std::streampos> LibroBuscador::procesar_archivo(const std::string &nombre_archivo) {
    std::ifstream fichero(nombre_archivo);
    std::vector<std::streampos> indice_lineas_archivo;
    indice_lineas_archivo.push_back(0); // index primera linea
    
    if (!fichero)
        throw std::runtime_error(std::format("Error al abrir el archivo {}", nombre_archivo));

    std::string linea_texto;
    while (std::getline(fichero, linea_texto)) {
        std::streampos posicion = fichero.tellg();
        if (posicion != -1) {
            indice_lineas_archivo.push_back(posicion);
        }
    }
    return indice_lineas_archivo;
}

// Detecta si un archivo es binario buscando bytes nulos (\0) en los primeros
// 8000 bytes, misma heurística que usa Git.
// Ref: https://github.com/git/git/blob/master/xdiff-interface
#define PRIMEROS_BYTES 8000
bool LibroBuscador::es_binario(const std::string &nombre_archivo) {
    std::ifstream f(nombre_archivo, std::ios::binary);
    char buffer[PRIMEROS_BYTES];
    std::streamsize leidos = f.read(buffer, sizeof(buffer)).gcount();
    return std::memchr(buffer, '\0', leidos) != nullptr;
}