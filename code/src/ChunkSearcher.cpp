/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ChunkSearcher.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 16/03/2026
 * Purpose      : Clase buscador que proporciona funcionalidad a los hilos, para localizar las palabras en los libros.
 * Revision     :
 * 16/03/2026 - Adrián - Creación del metodo read_file(), procesamiento de lineas del archivo. 
 * 18/03/2026 - Adrián - Creación de metodos para detectar matches validos y deteccion de palabras anteriores y posteriores. Incorporación de mutex.
 * 20/03/2026 - Adrián - Manejo de edge cases cuando la palabra buscada está al principio o al final de la linea
 * 27/03/2026 - Adrián - Manejo de edge cases cuando las palabras anterior o posterior se encuentran en porciones de otros hilos
 */

#include "../include/ChunkSearcher.hpp"
#include <format>
#include <iostream>

void ChunkSearcher::operator()() { 
    read_file();
}

// Procesamiento de la parte de archivo que corresponda, orquestando la busqueda linea por linea.
void ChunkSearcher::read_file() {
    std::ifstream file(file_name);
    if (!file) {
        std::cerr << std::format("[HILO {}] Error al abrir {}\n", thread_id, file_name);
        return;
    }

    file.seekg(start_line_byte);
    std::string line_text;
    std::string last_word = prepare_initial_context(file);

    for (int line_num = start_line; line_num <= end_line && std::getline(file, line_text); line_num++) {
        complete_pending_next_word(line_text);
        process_line(line_text, line_num, last_word);
        last_word = find_and_extract_word(line_text, (int)line_text.size(), Direction::BACKWARD);
    }

    fetch_next_chunk_word(file);
}

// Recupera la ultima palabra de la porcion anterior (de otro hilo)
std::string ChunkSearcher::prepare_initial_context(std::ifstream &file) {
    std::string line;
    if (start_line > 1 && std::getline(file, line)) {
        return find_and_extract_word(line, (int)line.size(), Direction::BACKWARD);
    }
    return "";
}

// Realiza la busqueda en una linea en concreto, usando los metodos auxiliares necesarios.
void ChunkSearcher::process_line(std::string &line, int line_num, const std::string &previous_context) {
    size_t pos = 0;
    while ((pos = line.find(target_word, pos)) != std::string::npos) {
        if (is_valid_match((int)pos, target_word, line)) {
            store_occurrence(line, pos, line_num, previous_context);
        }
        pos++;
    }
}

// Extrae la primera palabra de la siguiente porcion (de otro hilo) y guarda en ocurrencias
void ChunkSearcher::fetch_next_chunk_word(std::ifstream &file) {
    if (is_next_word_pending()) {
        std::string extra_next_line;
        if (std::getline(file, extra_next_line)) {
            complete_pending_next_word(extra_next_line);
        }
    }
}

void ChunkSearcher::store_occurrence(const std::string &line_text, size_t match_pos, int line_num,
                                    const std::string &last_word) {

    std::string provisional_prev_word = find_and_extract_word(line_text, match_pos - 1, Direction::BACKWARD);
    match_pos += target_word.length();
    std::string next_word = find_and_extract_word(line_text, match_pos, Direction::FORWARD);
    // Si no se ha encontrado palabra anterior, se recupera la ultima palabra de la linea anterior
    std::string previous = provisional_prev_word.empty() ? last_word : provisional_prev_word;

    std::lock_guard<std::mutex> lock(searcher_mutex);
    occurrences.emplace_back(line_num, previous, next_word);
}

// Si no se encuentra palabra anterior en la linea se completará con la ultima palabra de la linea anterior
void ChunkSearcher::complete_pending_next_word(std::string &line_text) {
    if (is_next_word_pending()) {
        std::string initial_word = find_and_extract_word(line_text, 0, Direction::FORWARD);
        std::lock_guard<std::mutex> lock(searcher_mutex);
        occurrences.back().set_next_word(initial_word);
    }
}

bool ChunkSearcher::is_next_word_pending() {
    std::lock_guard<std::mutex> lock(searcher_mutex);
    return !occurrences.empty() && occurrences.back().get_next_word().empty();
}

// Dada una posicion en la linea del texto y una direccion de recorrido, 
// recorre el texto y devuelve la primera palabra encontrada
std::string ChunkSearcher::find_and_extract_word(const std::string &line, int pos, Direction dir) {
    int cursor = pos;
    int step = static_cast<int>(dir);

    // Omitir separadores
    while (is_in_range(cursor, line) && is_separator(line[cursor]))
        cursor += step;

    if (!is_in_range(cursor, line))
        return "";

    // Identificar limites de la palabra
    int word_boundary = cursor;
    while (is_in_range(cursor, line) && !is_separator(line[cursor]))
        cursor += step;

    if (dir == Direction::FORWARD) {
        // [word_boundary] ... [cursor-1]
        return line.substr(word_boundary, cursor - word_boundary);
    } else {
        // [cursor+1] ... [word_boundary]
        return line.substr(cursor + 1, word_boundary - cursor);
    }
}

// Verifica que la coincidencia sea una palabra completa y no parte de otra (substring).
bool ChunkSearcher::is_valid_match(int match_pos, std::string &target_word, std::string &line) {
    int lower_bound = match_pos - 1;
    int upper_bound = match_pos + target_word.length();

    if (lower_bound >= 0 && !is_separator(line[lower_bound]))
        return false;

    if (upper_bound < (int)line.size() && !is_separator(line[upper_bound]))
        return false;

    return true;
}

// Un carácter es separador solo si NO es alfanumérico Y NO es multibyte (UTF-8).
// Eso permite que tildes y eñes se consideren parte de la palabra.
bool ChunkSearcher::is_separator(char c) {
    unsigned char character = (unsigned char)c;
    return !(isalnum(character) || character >= 0x80);
}

bool ChunkSearcher::is_in_range(int pos, const std::string &line) { return pos >= 0 && pos < (int)line.size(); }