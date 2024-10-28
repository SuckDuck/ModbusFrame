#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "modbusPayloader.h"

extern void __initReadPayload(MB_COMMAND_CODE commandCode,MB_payload_read* payload,uint8_t slave,uint16_t start,uint16_t numRegisters);
typedef struct {
    int functionCode;
    char* functionName;
} modbusFunction;

#define FUNCTIONS_IMPLEMENTED 4
modbusFunction valid_functions[] = {
    { 0x01, "read_coil_status"       },
    { 0x02, "read_input_status"      },
    { 0x03, "read_holding_registers" },
    { 0x04, "read_input_registers"    }
};

void help(){
    printf("Bare minimal utility for generating modbus-rtu frames\n");
    printf("Usage: modbusframe [slave] [function code | function name] [address] [address count] [-b]\n");
    printf("Options:\n");
    printf("[slave]              Modbus slave device id\n\n");
    printf("[function code]      Modbus function code id\n");

    printf("[function name]      Alternatively, function name can be used instead of code\n");
    printf("                     Available function names and codes are:\n");
    printf("                       CODE     NAME\n");
    printf("                       1        read_coil_status\n");
    printf("                       2        read_input_status\n");
    printf("                       3        read_holding_registers\n");
    printf("                       4        read_input_registers\n\n");

    printf("[address]            Starting register adress to read from\n");
    printf("[address count]      Numbers of registers to read\n");
    printf("[-b]                 (Optional) Print the frame in binary instead of hex\n");

}

void print_in_hex(void* payload, int payload_len){
    uint8_t* payload_ = (uint8_t*) payload;
    for(int i=0; i<payload_len; i++){
        if(*(payload_+i) <= 16){
            printf("0");
        }
        printf("%X",*(payload_+i));
    }
    printf("\n");
}

void print_in_bytes(void* payload, int payload_len){
    int b = 0;
    while(b < payload_len){
        b += write(1,payload,payload_len);
    }
}

// modbusframe [slave] [function code/function name] [adress] [No of adr]
int main(int argn, char** argv){
    if(argn < 5){
        help();
        return 1;
    }

    #pragma region // **************************** SLAVE *************************** //
    int slave = atoi(argv[1]);
    if(slave < 1){
        fprintf(stderr,"invalid slave index\n");
        return 1;
    }
    #pragma endregion

    #pragma region // ************************** FUNC CODE ************************* //
    int function_code = atoi(argv[2]);
    for(int i=0; i<FUNCTIONS_IMPLEMENTED+1; i++){
        if(i == FUNCTIONS_IMPLEMENTED){
            fprintf(stderr,"function code not valid or not implemented\n");
            return 1;
        }

        if(function_code == valid_functions[i].functionCode) break;
        if(strcmp(argv[2],valid_functions[i].functionName) == 0){
            function_code = valid_functions[i].functionCode;
            break;
        }
    }
    #pragma endregion

    #pragma region // *************************** ADRESS *************************** //
    int adress = atoi(argv[3]);
    if(adress < 0){
        fprintf(stderr,"invalid adress\n");
        return 1;
    }
    #pragma endregion

    #pragma region // *********************** REGISTER COUNT *********************** //
    int adress_count = atoi(argv[4]);
    if(adress_count <= 0){
        fprintf(stderr,"invalid adress count\n");
        return 1;
    }
    #pragma endregion

    MB_payload_read payload;
    __initReadPayload(function_code,&payload,slave,adress,adress_count);

    if(argn > 5 && strcmp(argv[5],"-b") == 0){
        print_in_bytes(&payload,sizeof(payload));
        return 0;
    }

    print_in_hex(&payload,sizeof(payload));
    return 0;
}