/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : SearchMonitor.cpp
 * Author		: Adrián Díaz-Plaza Remesal
 * Purpose      : Implementación de los monitores que controlan la busqueda de los diferentes tipos de clientes, junto a
 * su factoría.
 */

#include "payment/SearchMonitor.hpp"
#include "payment/PaymentProxy.hpp"
#include "utils/SystemConfig.hpp"
#include <atomic>
#include <condition_variable>
#include <format>
#include <iostream>
#include <mutex>
#include <stdexcept>

// Monitor para cuentas gratuitas: Corta el acceso de todos los hilos simultáneamente al alcanzar el límite
class FreeMonitor : public SearchMonitor {
  private:
	std::mutex mtx;
	int client_id;
	int words_found = 0;

	// Permite a los hilos buscadores comprobar si deben abortar sin el coste de adquirir el mutex
	std::atomic<bool> search_finished{false};

  public:
	FreeMonitor(int id) : client_id(id) {}

	bool should_stop() override { return search_finished; }

	void wait_for_payment() override {}

	bool register_match() override {
		// Doble comprobación, solo si la busqueda no se ha finalizado adquiere el mutex
		if (search_finished)
			return false;

		std::lock_guard<std::mutex> lock(mtx);

		// Si el contador de palabras encontradas alcanza el máximo permitido, 
		// se marca la búsqueda como finalizada para que los demás hilos se detengan eventualmente.
		if (words_found < Config::MAX_WORDS_FREE) {
			words_found++;
			if (words_found >= Config::MAX_WORDS_FREE) {
				search_finished = true;
				std::cout << std::format("[CLIENTE {}] Limite de ocurrencias alcanzado, finalizando busqueda...\n",
				                         client_id);
			}
			return true;
		}

		search_finished = true;
		return false;
	}
};

// Monitor para cuentas premium limitadas: Corta el acceso de todos los hilos simultáneamente al alcanzar el límite de saldo. Tras recarga, continúan
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

	// Si otro hilo está pagando, nos quedamos bloqueados aqui. 
	void wait_for_payment() override {
		if (is_paying) {
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [this] { return !is_paying; });
		}
	}

	// Si el saldo es 0, el primer hilo que llegue se encargará de hacer la recarga, mientras los demás esperan.
	bool register_match() override {
		std::unique_lock<std::mutex> lock(mtx);	
		while (is_paying || balance == 0) {
			if (is_paying) {
				cv.wait(lock, [this] { return !is_paying; });
			} else if (balance == 0) {
				is_paying = true;
				std::cout << std::format("[CLIENTE {}] Limite de saldo alcanzado, recargando...\n", client_id);

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

// Monitor para cuentas premium: Sin limites
class PremiumUnlimitedMonitor : public SearchMonitor {
  public:
	bool should_stop() override { return false; }
	void wait_for_payment() override {}
	bool register_match() override { return true; }
};

// Factoría de objetos que construye instancias según el tipo de cliente y sus parámetros asociados.
std::unique_ptr<SearchMonitor> SearchMonitor::create(int client_id, int initial_balance, ClientType type) {
	switch (type) {
	case ClientType::FREE:
		return std::make_unique<FreeMonitor>(client_id);

	case ClientType::PREMIUM_LIMITED:
		return std::make_unique<PremiumLimitedMonitor>(client_id, initial_balance);

	case ClientType::PREMIUM_UNLIMITED:
		return std::make_unique<PremiumUnlimitedMonitor>();

	default:
		throw std::invalid_argument("No existe el tipo de cliente");
	}
}