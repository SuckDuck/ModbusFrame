#include <stdio.h>
#include <stdint.h>
#include <modbusPayloader.h>

uint8_t slave = 1;
uint16_t start = 0;
uint16_t num_registers = 2;

int main(){
    //FOR READING HOLDING REGISTERS 1 AND 2 FROM SLAVE 1 DEVICE
    MB_payload_read_holding example_payload_h;
    slave = 1;
    start = 0;
    num_registers = 2;
    MB_initPayload_readHoldingReg(&example_payload_h,slave,start,num_registers);

    //FOR READING COILS 1,2 AND 3 FROM SLAVE 2 DEVICE
    MB_payload_read_coil example_payload_c;
    slave = 2;
    start = 0;
    num_registers = 3;
    MB_initPayload_readCoil(&example_payload_c,slave,start,num_registers);

    //FOR PARSING SOME RESPONSES TO ABOVE PAYLOADS
    //response buffer should be a point to a buffer containing a response
    //from a modbus device, captured some other way.
    uint8_t* response_buffer;
    int response_len;

    MB_response example_response;
    int rc = MB_initResponse(&example_response,response_buffer,response_len);
    if(rc != MB_OK){
        fprintf(stderr,"%s\n",MB_strerror(rc));    
        return 1;
    }

    //EXTRACT THE VALUES OF THE REGISTERS TO 'registers' POINTER,
    //SET 'registers_c' WITH THE REGISTERS COUNT
    uint16_t* registers;
    int registers_c;
    MB_getRegisters(&example_response,&registers,&registers_c);
    
    return 0;
}