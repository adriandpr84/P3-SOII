/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchTypes.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 24/04/2026
 * Purpose      : Define las estructuras y tipos que usará internamente el motor de busqueda.
 * Revision     :
 *
 */

#pragma once

#include "CommonTypes.hpp"
#include <future>

constexpr int SHUTDOWN = -33;
constexpr int LINES_PER_CHUNK = 2000;

struct InternalSearchRequest {
	SearchRequest data;
	std::promise<std::vector<BookResult>> promise{};

	InternalSearchRequest(SearchRequest req) : data(std::move(req)), promise() {}

	std::future<std::vector<BookResult>> get_future() { return promise.get_future(); }

	static InternalSearchRequest create_shutdown_request() {
		SearchRequest request = {SHUTDOWN, ClientType::FREE, ""};
		return InternalSearchRequest(std::move(request));
	}

	bool is_shutdown() const { return data.client_id == SHUTDOWN; }
};

struct BookMetadata {
	std::string path;
	std::vector<ChunkMetadata> line_index;
};

struct ChunkMetadata {
	int start_line;
	int end_line;
	std::streampos start_line_byte;
	bool last_chunk = false;
};

struct ChunkTask {
	int thread_id;
	int start_line;
	int end_line;
	std::streampos start_line_byte;
	std::string file_name;
	std::string target_word;
	std::promise<ChunkResult> promise{};

	ChunkTask(int id, int start, int end, std::streampos byte, std::string file, std::string word)
	    : thread_id(id), start_line(start), end_line(end), start_line_byte(byte), file_name(std::move(file)),
	      target_word(std::move(word)) {}

	static ChunkTask create_shutdown_task() {
        return ChunkTask(SHUTDOWN, 0, 0, 0, "", "");
    }

	bool is_shutdown() const { return thread_id == SHUTDOWN; }

	std::future<ChunkResult> get_future() { return promise.get_future(); }
};
