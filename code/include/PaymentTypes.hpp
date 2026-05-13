/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Program      : PaymentTypes.hpp
 * Purpose      : Define la estructura del mensaje para la cola System V (msgget).
 */

#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

constexpr key_t IPC_PAYMENT_KEY = 0x1234; 

constexpr long MSG_TYPE_REQUEST = 1; 

struct PaymentMessage {
    long mtype;             
    int client_id;          
    int recharged_balance;  
    bool is_shutdown;       
};