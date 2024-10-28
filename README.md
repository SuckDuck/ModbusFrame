# **ModbusFrame**

`modbusframe` is both a small tool for generating modbus-rtu payloads from a cli
and a bare minimum library for the same purpose, it is intended to be used when
creating gateways from other protocols to modbus-rtu or viceversa, as well as in 
development enviroments that require creating and/or saving modbus payloads for test
purposes.

Right now, only functions 01H, 02H, 03H and 04H are supported, but support to others will be added soon (i hope).

## Usage and examples
### **Cli**
Cli usage is pretty basic, if you run `modbusframe` without any params, it will
show a help message detailing the usage and options of the tool, however here are
some use cases:

* Some serial servers like [this one](https://www.waveshare.com/rs232-485-to-wifi-eth-b.htm)
let you configure a custom heartbeat for whatever device you are using, if
you want to use that feature for pulling some modbus registers, you can run
`modbusframe [args]` without the `-b` flag to get the payload in hexadecimal form,
then copy and paste the output into the field for the heartbeat content of your serial
server.

* If you have a development environment where you have to send fixed payloads
to a device when running tests, you could run something like 
`modbusframe [args] -b > custom_payload` to save the payload, then use
something like `cat custom_payload > /dev/tty[some port]` for sending the
payload, and then use screen with xxd or something to monitor the response.

### **Library**
library usage is also pretty simple, you can create different types of payloads and parse
responses if you have them. (`modbusframe` doesn't implement any communication phase with
the devices themselves, so you can't send payloads to devices or get their response using
this library, it is only intended to create and parse the data frames)

Here is a quick usage example:
````
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
    MB_initResponse(&example_response,response_buffer,response_len);
    
    //EXTRACT THE VALUES OF THE REGISTERS TO 'registers' POINTER,
    //SET 'registers_c' WITH THE REGISTERS COUNT
    uint16_t* registers;
    int registers_c;
    MB_getRegisters(&example_response,&registers,&registers_c);
    
    return 0;
}
````