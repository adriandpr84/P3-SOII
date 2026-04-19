/**
 * Project      : Practica 2 de Sistemas Operativos II
 * Program      : Buscador.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 16/03/2026
 * Purpose      : Clase buscador que proporciona funcionalidad a los hilos, para localizar las palabras en los libros.
 * Revision     :
 * 16/03/2026 - Adrián - Creación del metodo leer_archivo(), procesamiento de lineas del archivo. 
 * 18/03/2026 - Adrián - Creación de metodos para detectar matches validos y deteccion de palabras anteriores y posteriores. Incorporación de mutex.
 * 20/03/2026 - Adrián - Manejo de edge cases cuando la palabra buscada está al principio o al final de la linea
 * 27/03/2026 - Adrián - Manejo de edge cases cuando las palabras anterior o posterior se encuentran en porciones de otros hilos
 */

#include "../include/Buscador.hpp"
#include <format>
#include <iostream>

void Buscador::operator()() { leer_archivo(); }

// Procesamiento de la parte de archivo que corresponda, orquestando la busqueda linea por linea.
void Buscador::leer_archivo() {
	std::ifstream fichero(nombre_archivo);
	if (!fichero) {
		std::cerr << std::format("[HILO {}] Error al abrir {}\n", hilo_id, nombre_archivo);
		return;
	}

	fichero.seekg(byte_linea_inicio);
	std::string linea_texto;
	std::string ultima_palabra = preparar_contexto_inicial(fichero);

	for (int num_linea = linea_inicio; num_linea <= linea_fin && std::getline(fichero, linea_texto); num_linea++) {
		completar_palabra_posterior_pendiente(linea_texto);
		procesar_linea(linea_texto, num_linea, ultima_palabra);
		ultima_palabra = buscar_y_extraer_palabra(linea_texto, (int)linea_texto.size(), Direccion::ATRAS);
	}

	obtener_palabra_siguiente_porcion(fichero);
}


// Recupera la ultima palabra de la porcion anterior (de otro hilo)
std::string Buscador::preparar_contexto_inicial(std::ifstream &fichero) {
	std::string linea;
	if (linea_inicio > 1 && std::getline(fichero, linea)) {
		return buscar_y_extraer_palabra(linea, (int)linea.size(), Direccion::ATRAS);
	}
	return "";
}

// Realiza la busqueda en una linea en concreto, usando los metodos auxiliares necesarios.
void Buscador::procesar_linea(std::string &linea, int num_linea, const std::string &contexto_previo) {

	size_t pos = 0;
	while ((pos = linea.find(palabra_buscada, pos)) != std::string::npos) {
		if (es_coincidencia_valida((int)pos, palabra_buscada, linea)) {
			almacenar_ocurrencia(linea, pos, num_linea, contexto_previo);
		}
		pos++;
	}
}

// Extrae la primera palabra de la siguiente porcion (de otro hilo) y guarda en ocurrencias
void Buscador::obtener_palabra_siguiente_porcion(std::ifstream &fichero){
	if (palabra_posterior_pendiente()) {
        std::string siguiente_linea_extra;
        if (std::getline(fichero, siguiente_linea_extra)) {
            completar_palabra_posterior_pendiente(siguiente_linea_extra);
        }
    }
}




void Buscador::almacenar_ocurrencia(const std::string &linea_texto, size_t pos_coincidencia, int num_linea,
                                    const std::string &ultima_palabra) {

	std::string palabra_anterior_provisional = buscar_y_extraer_palabra(linea_texto, pos_coincidencia - 1, Direccion::ATRAS);
	pos_coincidencia += palabra_buscada.length();
	std::string palabra_posterior = buscar_y_extraer_palabra(linea_texto, pos_coincidencia, Direccion::ADELANTE);
	// Si no se ha encontrado palabra anterior, se recupera la ultima palabra de la linea anterior
	std::string anterior = palabra_anterior_provisional.empty() ? ultima_palabra : palabra_anterior_provisional;

	std::lock_guard<std::mutex> lock(mutex_buscador);
	ocurrencias.emplace_back(num_linea, anterior, palabra_posterior);
}

// Si no se encuentra palabra anterior en la linea se completará con la ultima palabra de la linea anterior
void Buscador::completar_palabra_posterior_pendiente(std::string &linea_texto) {
	if (palabra_posterior_pendiente()) {
		std::string palabra_inicial = buscar_y_extraer_palabra(linea_texto, 0, Direccion::ADELANTE);
		std::lock_guard<std::mutex> lock(mutex_buscador);
		ocurrencias.back().set_palabra_posterior(palabra_inicial);
	}
}

bool Buscador::palabra_posterior_pendiente (){
	std::lock_guard<std::mutex> lock(mutex_buscador);
	return !ocurrencias.empty() && ocurrencias.back().get_palabra_posterior().empty();
}




// Dada una posicion en la linea del texto y una direccion de recorrido, 
// recorre el texto y devuelve la primera palabra encontrada
std::string Buscador::buscar_y_extraer_palabra(const std::string &linea, int pos, Direccion dir) {
	int cursor = pos;
	int paso = static_cast<int>(dir);

	// Omitir separadores
	while (en_rango(cursor, linea) && es_separador(linea[cursor]))
		cursor += paso;

	if (!en_rango(cursor, linea))
		return "";

	// Identificar limites de la palabra
	int limite_palabra = cursor;
	while (en_rango(cursor, linea) && !es_separador(linea[cursor]))
		cursor += paso;

	if (dir == Direccion::ADELANTE) {
        // [limite_palabra] ... [cursor-1]
        return linea.substr(limite_palabra, cursor - limite_palabra);
    } else {
        // [cursor+1] ... [limite_palabra]
        return linea.substr(cursor + 1, limite_palabra - cursor);
    }
}


// Verifica que la coincidencia sea una palabra completa y no parte de otra (substring).
bool Buscador::es_coincidencia_valida(int pos_coincidencia, std::string &palabra_buscada, std::string &linea) {
	int limite_inferior = pos_coincidencia - 1;
	int limite_superior = pos_coincidencia + palabra_buscada.length();

	if (limite_inferior >= 0 && !es_separador(linea[limite_inferior]))
		return false;

	if (limite_superior < (int)linea.size() && !es_separador(linea[limite_superior]))
		return false;

	return true;
}

// Un carácter es separador solo si NO es alfanumérico Y NO es multibyte (UTF-8).
// Eso permite que tildes y eñes se consideren parte de la palabra.
bool Buscador::es_separador(char c) {
	unsigned char caracter = (unsigned char)c;
	return !(isalnum(caracter) || caracter >= 0x80);
}

bool Buscador::en_rango(int pos, const std::string &linea) { return pos >= 0 && pos < (int)linea.size(); }