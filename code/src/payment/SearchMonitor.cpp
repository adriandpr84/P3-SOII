/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchMonitor.cpp
 * Purpose      : Implementación oculta de las jerarquías de monitores y su Factoría.
 */

#include "payment/SearchMonitor.hpp"
#include "payment/PaymentProxy.hpp"
#include <condition_variable>
#include <mutex>

constexpr int MAX_WORDS_FREE = 20;

class FreeMonitor : public SearchMonitor {
  private:
	std::mutex mtx;
	int words_found = 0;
	std::atomic<bool> search_finished{false};

  public:
	bool should_stop() override { return search_finished; }

	void wait_for_payment() override {}

	bool register_match() override {
		if (search_finished)
			return false;

		std::lock_guard<std::mutex> lock(mtx);

		if (words_found < MAX_WORDS_FREE) {
			words_found++;
			if (words_found >= MAX_WORDS_FREE) {
				search_finished = true;
			}
			return true;
		}

		search_finished = true;
		return false;
	}
};

class PremiumLimitedMonitor : public SearchMonitor {
  private:
	std::mutex mtx;
	std::condition_variable cv;
	int client_id;
	int balance;
	std::atomic<bool> is_paying{false};

  public:
	PremiumLimitedMonitor(int id, int initial_balance) : client_id(id), balance(initial_balance) {}

	bool should_stop() override { return false; }

	void wait_for_payment() override {
		if (is_paying) {
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [this] { return !is_paying; });
		}
	}

	bool register_match() override {
		std::unique_lock<std::mutex> lock(mtx);

		while (is_paying || balance == 0) {
			if (is_paying) {
				cv.wait(lock, [this] { return !is_paying; });
			} else if (balance == 0) {
				is_paying = true;
				lock.unlock();

				int new_balance = PaymentProxy::get_instance().process_payment(client_id);

				lock.lock();
				balance = new_balance;
				is_paying = false;
				cv.notify_all();
			}
		}

		balance--;
		return true;
	}
};

class PremiumUnlimitedMonitor : public SearchMonitor {
  public:
	bool should_stop() override { return false; }
	void wait_for_payment() override {}
	bool register_match() override { return true; }
};

std::unique_ptr<SearchMonitor> SearchMonitor::create(int client_id, int initial_balance, ClientType type) {
	switch (type) {
	case ClientType::FREE:
		return std::make_unique<FreeMonitor>();

	case ClientType::PREMIUM_LIMITED:
		return std::make_unique<PremiumLimitedMonitor>(client_id, initial_balance);

	case ClientType::PREMIUM_UNLIMITED:
		return std::make_unique<PremiumUnlimitedMonitor>();

	default:
		throw std::invalid_argument("No existe el tipo");
	}
}