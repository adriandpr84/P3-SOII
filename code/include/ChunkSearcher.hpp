#pragma once

#include "ConcurrentQueue.hpp"
#include "Occurrence.hpp"
#include "SearchResultsTypes.hpp"
#include "SearchTypes.hpp"
#include <condition_variable>
#include <fstream>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

class ChunkSearcher {
  private:
	enum class Direction { BACKWARD = -1, FORWARD = 1 };

	ConcurrentQueue<ChunkTask> &task_queue;

	ChunkResult read_file(const ChunkTask &task);
	std::string prepare_initial_context(std::ifstream &file, int start_line);

	void process_line(std::string &line, int line_num, const std::string &previous_context, const ChunkTask &task,
	                  ChunkResult &result);

	void fetch_next_chunk_word(std::ifstream &file, ChunkResult &result);

	void store_occurrence(const std::string &line_text, size_t match_pos, int line_num, const std::string &last_word,
	                      const ChunkTask &task, ChunkResult &result);

	void complete_pending_next_word(std::string &line_text, ChunkResult &result);

	bool is_next_word_pending(const ChunkResult &result);

	std::string find_and_extract_word(const std::string &line, int pos, Direction dir);
	bool is_valid_match(int pos, const std::string &target_word, const std::string &line);
	bool is_separator(char c);
	bool is_in_range(int pos, const std::string &line);

  public:
	ChunkSearcher(ConcurrentQueue<ChunkTask> &_task_queue) : task_queue(_task_queue) {}

	void operator()();
};