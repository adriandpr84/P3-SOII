cmake_minimum_required(VERSION 3.14)
project(SSOOIIGLE_Tests CXX)


set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Werror -g)


include_directories(../include)


find_package(Threads REQUIRED)


enable_testing()


add_executable(test_scheduler test_priority_scheduler.cpp ../src/PriorityScheduler.cpp)
target_link_libraries(test_scheduler PRIVATE Threads::Threads)


add_test(NAME Scheduler_FIFO        COMMAND test_scheduler FIFO)
add_test(NAME Scheduler_Fallback    COMMAND test_scheduler FALLBACK)
add_test(NAME Scheduler_Ratio_80_20 COMMAND test_scheduler RATIO)
add_test(NAME Scheduler_Concurrency      COMMAND test_scheduler CONCURRENCY)
add_test(NAME Scheduler_Starvation      COMMAND test_scheduler STARVATION)


add_executable(test_search_engine 
    test_search_engine.cpp 
    ../src/SearchManager.cpp
    ../src/SearchService.cpp
    ../src/ChunkSearcher.cpp
    ../src/LibraryIndexer.cpp
    ../src/PriorityScheduler.cpp
)
target_link_libraries(test_search_engine PRIVATE Threads::Threads)


add_test(NAME Engine_Concurrency COMMAND test_search_engine concurrency)


add_test(NAME Engine_Shutdown_Race_Condition COMMAND test_search_engine shutdown)


set_tests_properties(Engine_Shutdown_Race_Condition PROPERTIES TIMEOUT 10)