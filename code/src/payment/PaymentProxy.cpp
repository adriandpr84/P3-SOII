/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : PaymentProxy.cpp
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Purpose      : Implementación del proxy de pagos usando colas de mensajes.
 */

#include "payment/PaymentProxy.hpp"
#include "utils/PaymentTypes.hpp"
#include <iostream>
#include <stdexcept>

PaymentProxy::PaymentProxy() : msgid(-1) {}

void PaymentProxy::init() {
    msgid = msgget(IPC_PAYMENT_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        throw std::runtime_error("[PROXY PAGO] Error fatal: No se pudo conectar a la cola IPC.");
    }
}

int PaymentProxy::process_payment(int client_id) {
    PaymentMessage msg;
    msg.mtype = MSG_TYPE_REQUEST; 
    msg.client_id = client_id;
    msg.recharged_balance = 0;
    msg.is_shutdown = false;

    if (msgsnd(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), 0) == -1) {
        std::cerr << "[PROXY PAGO] Error al enviar petición para el cliente " << client_id << "\n";
        return 0;
    }

    if (msgrcv(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), client_id + RESPONSE_OFFSET, 0) == -1) {
        std::cerr << "[PROXY PAGO] Error al recibir respuesta para el cliente " << client_id << "\n";
        return 0;
    }

    return msg.recharged_balance;
}

void PaymentProxy::shutdown_remote_system() {
    PaymentMessage msg;
    msg.mtype = MSG_TYPE_REQUEST;
    msg.is_shutdown = true;
    
    msgsnd(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), 0);
}