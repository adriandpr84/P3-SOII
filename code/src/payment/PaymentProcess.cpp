/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Autor        : Manuel Martín-Tembleque García-Serrano.
 * Program      : PaymentProcess.cpp
 * Purpose      : Implementación del servidor de pagos independiente.
 */

#include "payment/PaymentProcess.hpp"
#include "utils/PaymentTypes.hpp"
#include <iostream>
#include <format>
#include <chrono>
#include <thread>
#include <unistd.h>

void PaymentProcess::run_server() {
    std::cout << std::format("[SISTEMA PAGO] Iniciando proceso independiente (PID: {})...\n", getpid());

    int msgid = msgget(IPC_PAYMENT_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        std::cerr << "[SISTEMA PAGO] Error fatal al crear/acceder a la cola IPC.\n";
        return;
    }

    std::cout << "[SISTEMA PAGO] Escuchando peticiones en la cola...\n";

    while (true) {
        PaymentMessage msg;

        if (msgrcv(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), MSG_TYPE_REQUEST, 0) == -1) {
            std::cerr << "[SISTEMA PAGO] Error al recibir mensaje IPC.\n";
            continue;
        }

        if (msg.is_shutdown) {
            std::cout << "[SISTEMA PAGO] Recibida orden de apagado. Cerrando sistema.\n";
            break;
        }

        std::cout << std::format("[SISTEMA PAGO] Procesando recarga para el cliente {}...\n", msg.client_id);

        msg.mtype = msg.client_id; 
        msg.recharged_balance = 10; 

        std::cout << std::format("[SISTEMA PAGO] Enviando {} créditos al cliente {}.\n", msg.recharged_balance, msg.client_id);

        msgsnd(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), 0);
    }

    msgctl(msgid, IPC_RMID, nullptr);
    std::cout << "[SISTEMA PAGO] Cola IPC destruida. Proceso finalizado.\n";
}