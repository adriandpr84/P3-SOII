#include "utils/CommonTypes.hpp"
#include "search/SearchManager.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;
const std::string TEST_PATH = "test_books_dir";
const std::vector<PriorityEntry> priority_config = {{80, {ClientType::PREMIUM_LIMITED, ClientType::PREMIUM_UNLIMITED}},
	                                              {20, {ClientType::FREE}}};

void setup_library(int num_books, int lines_per_book, int words_per_line) {
    if (fs::exists(TEST_PATH)) fs::remove_all(TEST_PATH);
    fs::create_directory(TEST_PATH);
    
    for (int book_id = 0; book_id < num_books; ++book_id) {
        std::string file_path = TEST_PATH + "/libro_" + std::to_string(book_id) + ".txt";
        std::ofstream file(file_path);
        
        for (int i = 0; i < lines_per_book; ++i) {
            file << "Línea " << i << " PalabraObjetivo";
            
            for (int w = 3; w < words_per_line; ++w) {
                file << " extra";
            }
            file << ".\n"; 
        }
        file.close();
    }
}


int run_test_concurrency() {
    setup_library(20, 5000, 100);
    constexpr int NUM_CLIENTS = 4;
    constexpr int NUM_REQUESTS = 50;

    auto &manager = SearchManager::get_instance();
	
    manager.init(TEST_PATH, priority_config);

    std::atomic<int> success{0};
    std::vector<std::thread> clients;
    
    for (int i = 0; i < NUM_CLIENTS; ++i) {
        clients.emplace_back([&, i]() {
            for (int j = 0; j < NUM_REQUESTS; ++j) {
                try {

                    ClientType type = (j % 2 == 0) ? ClientType::FREE : ClientType::PREMIUM_UNLIMITED;
                    
                    int client_id = (i * 100) + j; 

                    auto res = manager.execute_search({client_id, type, "PalabraObjetivo", 0});
                    
                    if (!res.results_per_book.empty()) {
                        success++;
                    }
                } catch (...) {}
            }
        });
    }
    
    for (auto &t : clients) t.join();
    manager.shutdown();
    
    bool ok = (success == NUM_CLIENTS * NUM_REQUESTS);
    return ok ? 0 : 1;
}

int run_test_shutdown() {
    setup_library(1, 100, 4);
    auto &manager = SearchManager::get_instance();
    manager.init(TEST_PATH, priority_config);

    std::atomic<bool> keep_going{true};
    const int NUM_ATTACKERS = 50; 
    std::vector<std::thread> attackers;

    for (int i = 0; i < NUM_ATTACKERS; ++i) {
        attackers.emplace_back([&, i]() {
            while (keep_going) {
                try {
                    manager.execute_search({2000 + i, ClientType::PREMIUM_UNLIMITED, "PalabraObjetivo", 0});
                } catch (...) {
                }
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    manager.shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    keep_going = false;

    for (auto &t : attackers) {
        if (t.joinable()) {
            t.join();
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " [concurrency|shutdown]\n";
        return 1;
    }
    std::string test_name = argv[1];
    if (test_name == "concurrency") return run_test_concurrency();
    if (test_name == "shutdown")    return run_test_shutdown();
    return 1;
}