#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "modbusPayloader.h"

#pragma region UTILITIES

uint16_t swapEndian16(uint16_t in){
    return (in << 8) | (in >> 8);
}

uint16_t getCR16(const uint8_t *data, int length) {
    uint16_t crc = 0xFFFF;  // Valor inicial del CRC
    for (int i = 0; i < length; i++) {
        crc ^= data[i];  // XOR del byte de datos con el CRC
        for (int j = 0; j < 8; j++) {  // Iterar sobre cada bit
            if (crc & 0x0001) {  // Si el bit menos significativo es 1
                crc >>= 1;  // Desplazar CRC a la derecha
                crc ^= 0xA001;  // XOR con el polinomio
            } else {
                crc >>= 1;  // Solo desplazar si el bit es 0
            }
        }
    }
    return crc; 
}

void divideUint16(uint8_t* out, uint16_t in){
    *out = (uint8_t)(in >> 8);
    *(out+1) = (uint8_t) in & 0xFF;
}

#pragma endregion

void __initReadPayload(MB_COMMAND_CODE commandCode,MB_payload_read* payload,uint8_t slave,uint16_t start,uint16_t numRegisters){
        payload->slaveAddr = slave;
        payload->functionCode = commandCode;
        payload->startAdress = swapEndian16(start);
        payload->nRegisters = swapEndian16(numRegisters);
        payload->crc16 = getCR16((uint8_t*) &payload->slaveAddr,6);
}

void MB_initPayload_readHoldingReg(MB_payload_read_holding* payload,uint8_t slave,uint16_t start,uint16_t numRegisters){
    __initReadPayload(MB_COMMAND_READ_HOLDING_REG,payload,slave,start,numRegisters);
}

void MB_initPayload_readInputReg(MB_payload_read_input_reg* payload,uint8_t slave,uint16_t start,uint16_t numRegisters){
    __initReadPayload(MB_COMMAND_READ_INPUT_REG,payload,slave,start,numRegisters);
}

void MB_initPayload_readCoil(MB_payload_read_coil* payload,uint8_t slave,uint16_t start,uint16_t numRegisters){
    __initReadPayload(MB_COMMAND_READ_COIL,payload,slave,start,numRegisters);
}

void MB_initPayload_readInputStat(MB_payload_read_input_stat* payload,uint8_t slave,uint16_t start,uint16_t numRegisters){
    __initReadPayload(MB_COMMAND_READ_INPUT_STAT,payload,slave,start,numRegisters);
}

char* MB_strerror(int error){
    switch(error){
        case MB_OK:                     return "MB_OK";
        case MB_ILLEGAL_FUNCTION:       return "MB_ILLEGAL_FUNCTION";
        case MB_ILLEGAL_DATA_ADDRESS:   return "MB_ILLEGAL_DATA_ADDRESS";
        case MB_ILLEGAL_DATA_VALUE:     return "MB_ILLEGAL_DATA_VALUE";
        case MB_SLAVE_DEVICE_FAILURE:   return "MB_SLAVE_DEVICE_FAILURE";
        case MB_SLAVE_DEVICE_BUSY:      return "MB_SLAVE_DEVICE_BUSY";
        case MB_INVALID_PAYLOAD:        return "MB_INVALID_PAYLOAD";
    }
}

int MB_initResponse(MB_response* response, uint8_t* data, int data_len){
    //response->data SHOULD BE FREED ONCE DONE
    if(data_len < 5) return MB_INVALID_PAYLOAD;
    if(data_len == 5){ //RESPONSE IS AN ERROR
        switch(*(data+2)){
            case 0x01:  return MB_ILLEGAL_FUNCTION;
            case 0x02:  return MB_ILLEGAL_DATA_ADDRESS;
            case 0x03:  return MB_ILLEGAL_DATA_VALUE;
            case 0x04:  return MB_SLAVE_DEVICE_FAILURE;
            case 0x06:  return MB_SLAVE_DEVICE_BUSY;
            default:    return MB_INVALID_PAYLOAD;
        }
    }

    //CRC16 CHECK
    response->crc16 = *(uint16_t*) (data+data_len-2);
    if(response->crc16 != getCR16(data,data_len-2)){
        return MB_CORRUPTED_RESPONSE;
    }

    //SET STATIC REGS
    response->slaveAddr = *(data+0);
    response->functionCode = *(data+1);
    response->responseLen = *(data+2);

    //SET DYNAMIC DATA
    response->data = (uint8_t*) malloc(response->responseLen);
    for(int i=0; i<response->responseLen;i++){
        *(response->data +i) = *(data+3+i);
    }

    return MB_OK;
}

void MB_getRegisters(MB_response* response ,uint16_t** registers, int* registerCount){
    //INDIVIDUAL REGISTERS SHOULD BE USED WITH "swapEndian16"
    *registerCount = response->responseLen/2;
    *registers = (uint16_t*) response->data;
}
