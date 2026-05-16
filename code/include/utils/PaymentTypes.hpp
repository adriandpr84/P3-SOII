/**
 * Project      : Practica 3 de Sistemas Operativos II
 * Author       : Manuel Martín-Tembleque García-Serrano.
 * Program      : PaymentTypes.hpp
 * Purpose      : Define la estructura del mensaje para la cola System V (msgget).
 */

#pragma once

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

constexpr key_t IPC_PAYMENT_KEY = 0xADA99; 

constexpr long MSG_TYPE_REQUEST = 1; 
constexpr long RESPONSE_OFFSET = 10000; 

struct PaymentMessage {
    long mtype;             
    int client_id;          
    int recharged_balance;  
    bool is_shutdown;       
};