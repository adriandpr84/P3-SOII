/**
 * Project      : Practica 2 de Sistemas Operativos II
 * Program      : Buscador.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 02/03/2026
 * Purpose      : Archivo de cabecera de la clase Ocurrencia que almacena las coincidencias del texto
 * Revision     :
 * 02/03/2026 - Adrián - Creación de la estructura de la clase Ocurrencia según lo definido en clase 
 * Desde el 16/03/2026 hasta el 27/03/2026 se han ido añadiendo getters y setters según se han ido requiriendo.
 */


#ifndef OCURRENCIA_HPP
#define OCURRENCIA_HPP

#include <string>

class Ocurrencia {
  private:
	int num_linea;
	std::string palabra_anterior;
	std::string palabra_posterior;

  public:
	Ocurrencia(int _num_linea, std::string _palabra_anterior, std::string _palabra_posterior)
	    : num_linea(_num_linea), palabra_anterior(_palabra_anterior), palabra_posterior(_palabra_posterior) {}

	int get_num_linea() const { return num_linea; }
	const std::string &get_palabra_anterior() const { return palabra_anterior; }
	const std::string &get_palabra_posterior() const { return palabra_posterior; }
	void set_palabra_posterior(std::string palabra) { palabra_posterior = std::move(palabra); }
};

#endif