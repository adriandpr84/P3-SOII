cmake_minimum_required(VERSION 3.14)
project(SSOOIIGLE_Tests CXX)


set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Werror -g)


include_directories(../include)


find_package(Threads REQUIRED)


enable_testing()


add_executable(test_scheduler test_priority_scheduler.cpp ../src/PriorityScheduler.cpp)
target_link_libraries(test_scheduler PRIVATE Threads::Threads)

# Registramos los tests individualmente
add_test(NAME Scheduler_FIFO        COMMAND test_scheduler FIFO)
add_test(NAME Scheduler_Fallback    COMMAND test_scheduler FALLBACK)
add_test(NAME Scheduler_Ratio_80_20 COMMAND test_scheduler RATIO)
add_test(NAME Scheduler_Concurrency      COMMAND test_scheduler CONCURRENCY)
add_test(NAME Scheduler_Starvation      COMMAND test_scheduler STARVATION)