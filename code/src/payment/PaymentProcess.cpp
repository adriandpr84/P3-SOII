/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Autor        : Manuel Martín-Tembleque García-Serrano.
 * Program      : PaymentProcess.cpp
 * Purpose      : Implementación del servidor de pagos independiente. 
 * Este proceso se encarga de escuchar las peticiones de recarga de los clientes 
 * y responder con la cantidad recargada. Se comunica con los clientes 
 * a través de una cola IPC. El proceso se detiene al recibir una señal de apagado.
 */

#include "payment/PaymentProcess.hpp"
#include "utils/PaymentTypes.hpp"
#include "utils/SystemConfig.hpp"
#include <iostream>
#include <format>
#include <chrono>
#include <thread>
#include <unistd.h>
// Método principal del proceso de pagos, que se ejecuta en un proceso independiente.
void PaymentProcess::run_server() {
    std::cout << std::format("[SISTEMA PAGO] Iniciando proceso independiente (PID: {})...\n", getpid());
    // Asegurarse de limpiar cualquier cola IPC existente antes de crear una nueva.
    int msgid_old = msgget(IPC_PAYMENT_KEY, 0666);
    if (msgid_old != -1) {
        msgctl(msgid_old, IPC_RMID, nullptr); 
    }

    int msgid = msgget(IPC_PAYMENT_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        std::cerr << "[SISTEMA PAGO] Error fatal al crear/acceder a la cola IPC.\n";
        return;
    }

    std::cout << "[SISTEMA PAGO] Escuchando peticiones de pago...\n";

    while (true) {
        PaymentMessage msg;
        // Esperar a recibir una petición de pago. El proceso se bloquea aquí hasta que llegue un mensaje.
        if (msgrcv(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), MSG_TYPE_REQUEST, 0) == -1) {
            std::cerr << "[SISTEMA PAGO] Error al recibir mensaje IPC.\n";
            continue;
        }

        if (msg.is_shutdown) {
            std::cout << "[SISTEMA PAGO] Recibida orden de apagado. Cerrando sistema.\n";
            break;
        }
        
        msg.mtype = msg.client_id + RESPONSE_OFFSET; 
        msg.recharged_balance = Config::RECHARGE_AMOUNT; 

        std::cout << std::format("[SISTEMA PAGO] Enviando {} créditos al cliente {}.\n", msg.recharged_balance, msg.client_id);

        msgsnd(msgid, &msg, sizeof(PaymentMessage) - sizeof(long), 0);
    }
    // Limpiar la cola IPC antes de finalizar el proceso.
    msgctl(msgid, IPC_RMID, nullptr);
    std::cout << "[SISTEMA PAGO] Cola IPC destruida. Proceso finalizado.\n";
}