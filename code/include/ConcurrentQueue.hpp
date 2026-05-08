/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : ConcurrentQueue.hpp
 * Author       : Adrián Díaz-Plaza Remesal
 * Date created : 30/04/2026
 * Purpose      : //TODO descripcion
 * Revision     :
 *
 */


#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class ConcurrentQueue {
  private:
	std::queue<T> queue;
	std::mutex mtx;
	std::condition_variable cv;

  public:
	ConcurrentQueue() = default;

	void push(T item) {
		std::lock_guard<std::mutex> lock(mtx);
		queue.push(std::move(item));
		cv.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this] { return !queue.empty(); });
		T item = std::move(queue.front());
		queue.pop();
		return item;
	}
};