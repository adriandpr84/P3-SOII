/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ChunkSearcher.cpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 16/03/2026
 * Purpose      : Clase buscador que proporciona funcionalidad a los hilos, para localizar las palabras en los libros.
 */

#include "ChunkSearcher.hpp"

#include <format>
#include <iostream>

void ChunkSearcher::operator()() {
	while (true) {
		ChunkTask task = task_queue.pop();

		if (task.is_shutdown())
			break;
		try {
			ChunkResult result = read_file(task);
			task.promise.set_value(std::move(result));
		} catch (const std::exception &e) {
			ChunkResult error_result;
			error_result.thread_id = task.thread_id;
			error_result.start_line = task.start_line;
			error_result.end_line = task.end_line;
			error_result.error_msg = e.what();

			task.promise.set_value(std::move(error_result));
		}
	}
}

// Procesamiento de la parte de archivo que corresponda, orquestando la busqueda linea por linea.
ChunkResult ChunkSearcher::read_file(const ChunkTask &task) {
	ChunkResult result;
	result.thread_id = task.thread_id;
	result.start_line = task.start_line;
	result.end_line = task.end_line;

	std::ifstream file(task.file_name);
	if (!file) {
		throw std::runtime_error(std::format("No se pudo abrir el libro"));
	}

	file.seekg(task.start_line_byte);
	std::string line_text;
	std::string last_word = prepare_initial_context(file, task.start_line);

	for (int line_num = task.start_line; line_num <= task.end_line && std::getline(file, line_text); line_num++) {
		complete_pending_next_word(line_text, result);
		process_line(line_text, line_num, last_word, task, result);
		last_word = find_and_extract_word(line_text, (int)line_text.size(), Direction::BACKWARD);
	}

	fetch_next_chunk_word(file, result);

	return result;
}

// Recupera la ultima palabra de la porcion anterior (de otro hilo)
std::string ChunkSearcher::prepare_initial_context(std::ifstream &file, int start_line) {
	std::string line;
	if (start_line > 1 && std::getline(file, line)) {
		return find_and_extract_word(line, (int)line.size(), Direction::BACKWARD);
	}
	return "";
}

// Realiza la busqueda en una linea en concreto, usando los metodos auxiliares necesarios.
void ChunkSearcher::process_line(std::string &line, int line_num, const std::string &previous_context,
                                 const ChunkTask &task, ChunkResult &result) {
	size_t pos = 0;
	while ((pos = line.find(task.target_word, pos)) != std::string::npos) {
		if (is_valid_match((int)pos, task.target_word, line)) {
			store_occurrence(line, pos, line_num, previous_context, task, result);
		}
		pos++;
	}
}

// Extrae la primera palabra de la siguiente porcion (de otro hilo) y guarda en ocurrencias
void ChunkSearcher::fetch_next_chunk_word(std::ifstream &file, ChunkResult &result) {
	if (is_next_word_pending(result)) {
		std::string extra_next_line;
		if (std::getline(file, extra_next_line)) {
			complete_pending_next_word(extra_next_line, result);
		}
	}
}

void ChunkSearcher::store_occurrence(const std::string &line_text, size_t match_pos, int line_num,
                                     const std::string &last_word, const ChunkTask &task, ChunkResult &result) {

	std::string provisional_prev_word = find_and_extract_word(line_text, match_pos - 1, Direction::BACKWARD);
	match_pos += task.target_word.length();
	std::string next_word = find_and_extract_word(line_text, match_pos, Direction::FORWARD);

	// Si no se ha encontrado palabra anterior, se recupera la ultima palabra de la linea anterior
	std::string previous = provisional_prev_word.empty() ? last_word : provisional_prev_word;

	result.occurrences.emplace_back(line_num, previous, next_word);
}

// Si no se encuentra palabra anterior en la linea se completará con la ultima palabra de la linea anterior
void ChunkSearcher::complete_pending_next_word(std::string &line_text, ChunkResult &result) {
	if (is_next_word_pending(result)) {
		std::string initial_word = find_and_extract_word(line_text, 0, Direction::FORWARD);
		result.occurrences.back().set_next_word(initial_word);
	}
}

bool ChunkSearcher::is_next_word_pending(const ChunkResult &result) {
	return !result.occurrences.empty() && result.occurrences.back().get_next_word().empty();
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
bool ChunkSearcher::is_valid_match(int match_pos, const std::string &target_word, const std::string &line) {
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