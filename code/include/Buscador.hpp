/**
 * Project      : Practica 2 de Sistemas Operativos II
 * Program      : Buscador.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 02/03/2026
 * Purpose      : Archivo de cabecera de la clase Buscador
 * Revision     :
 * 02/03/2026 - Adrián - Creación de la estructura de la clase Buscador según lo definido en clase 
 * Desde el 16/03/2026 hasta el 27/03/2026 se han ido añadiendo metodos necesarios a la clase 
 * según se desarrollaba funcionalidad y se refactorizaba en el Buscador.cpp
 */


#ifndef BUSCADOR_HPP
#define BUSCADOR_HPP

#include "Ocurrencia.hpp"
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

class Buscador {

  private:
	enum class Direccion { ATRAS = -1, ADELANTE = 1 };

	// Variables
	int hilo_id;
	int linea_inicio;
	int linea_fin;
	std::streampos byte_linea_inicio;
	std::string nombre_archivo;
	std::string palabra_buscada;
	std::vector<Ocurrencia> ocurrencias;
	std::mutex &mutex_buscador;

	// Métodos
	void leer_archivo();

	std::string preparar_contexto_inicial(std::ifstream &fichero);
	void procesar_linea(std::string &linea, int num_linea, const std::string &contexto_previo);
	void obtener_palabra_siguiente_porcion(std::ifstream &fichero);

	void almacenar_ocurrencia(const std::string &linea_texto, size_t pos_coincidencia, int num_linea,
	                          const std::string &ultima_palabra);
	void completar_palabra_posterior_pendiente(std::string &linea_texto);
	bool palabra_posterior_pendiente();

	std::string buscar_y_extraer_palabra(const std::string &linea, int pos, Direccion dir);
	bool es_coincidencia_valida(int pos, std::string &palabra_buscada, std::string &linea);
	bool es_separador(char c);
	bool en_rango(int pos, const std::string &linea);

  public:
	Buscador(int _hilo_id, int _inicio, int _final, std::streampos _byte_inicio, std::string _nombre_archivo,
	         std::string _palabra_buscada, std::mutex &_mutex_buscador)
	    : hilo_id(_hilo_id), linea_inicio(_inicio), linea_fin(_final), byte_linea_inicio(_byte_inicio),
	      nombre_archivo(_nombre_archivo), palabra_buscada(_palabra_buscada), mutex_buscador(_mutex_buscador) {}
	void operator()();

	// Getters
	std::vector<Ocurrencia> get_ocurrencias() const { return ocurrencias; }
	int get_hilo_id() const { return hilo_id; }
	int get_inicio() const { return linea_inicio; };
	int get_final() const { return linea_fin; };
	std::string get_palabra_buscada() const { return palabra_buscada; }
};

#endif