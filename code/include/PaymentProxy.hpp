/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : PaymentProxy.hpp
 * Purpose      : Interfaz en el Proceso Padre para comunicarse con el Proceso Hijo de pagos.
 */

#pragma once

class PaymentProxy {
private:
    int msgid;
    PaymentProxy(); 

public:
    static PaymentProxy& get_instance() {
        static PaymentProxy instance;
        return instance;
    }

    // Prevención de copias
    PaymentProxy(const PaymentProxy&) = delete;
    PaymentProxy& operator=(const PaymentProxy&) = delete;

    void init();
    int process_payment(int client_id); // Este metodo lo tiene que llamar la clase ChunkSeacher.
    void shutdown_remote_system();
};