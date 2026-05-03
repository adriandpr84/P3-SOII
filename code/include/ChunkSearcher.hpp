/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ChunkSearcher.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 02/03/2026
 * Purpose      : Archivo de cabecera de la clase ChunkSearcher
 * Revision     :
 * 02/03/2026 - Adrián - Creación de la estructura de la clase ChunkSearcher según lo definido en clase
 * Desde el 16/03/2026 hasta el 27/03/2026 se han ido añadiendo metodos necesarios a la clase
 * según se desarrollaba funcionalidad y se refactorizaba en el ChunkSearcher.cpp
 */

#pragma once

#include "Occurrence.hpp"
#include "SearchResultsTypes.hpp"
#include <fstream>
#include <semaphore>

#include <condition_variable>
#include <mutex>

#include <queue>
#include <string>
#include <vector>

struct ChunkTask {
	int thread_id;
	int start_line;
	int end_line;
	std::streampos start_line_byte;
	std::string file_name;
	std::string target_word;
	ChunkResult &result;
};

class ChunkSearcher {

  private:
	enum class Direction { BACKWARD = -1, FORWARD = 1 };

	std::mutex &task_queue_mtx;
	std::condition_variable &cv_empty_queue;
	std::queue<ChunkTask> &chunks_task_queue;

	// Métodos
	void read_file(ChunkTask task);

	std::string prepare_initial_context(std::ifstream &file, int start_line);
	void process_line(std::string &line, int line_num, const std::string &previous_context,
	                  const std::string &target_word);
	void fetch_next_chunk_word(std::ifstream &file, std::vector<Occurrence> &ocurrences);

	void store_occurrence(const std::string &line_text, size_t match_pos, int line_num, const std::string &last_word,
	                      const std::string &target_word, std::vector<Occurrence> &ocurrences);
	void complete_pending_next_word(std::string &line_text, std::vector<Occurrence> &ocurrences);
	bool is_next_word_pending(std::vector<Occurrence> &ocurrences);

	std::string find_and_extract_word(const std::string &line, int pos, Direction dir);
	bool is_valid_match(int pos, const std::string &target_word, std::string &line);
	bool is_separator(char c);
	bool is_in_range(int pos, const std::string &line);

  public:
	ChunkSearcher(std::mutex &_task_queue_mtx, std::condition_variable &_cv_empty_queue,
	              std::queue<ChunkTask> &_chunks_task_queue)
	    : task_queue_mtx(_task_queue_mtx), cv_empty_queue(_cv_empty_queue), chunks_task_queue(_chunks_task_queue) {};
	void operator()();
};
