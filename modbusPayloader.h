#ifndef MODBUS_PAYLOADER
#define MODBUS_PAYLOADER

enum MB_ERROR{
    MB_OK,
    MB_ILLEGAL_FUNCTION,
    MB_ILLEGAL_DATA_ADDRESS,
    MB_ILLEGAL_DATA_VALUE,
    MB_SLAVE_DEVICE_FAILURE,
    MB_SLAVE_DEVICE_BUSY,
    MB_INVALID_PAYLOAD,
    MB_CORRUPTED_RESPONSE
};

#pragma region COMMAND_CODES

typedef uint8_t MB_COMMAND_CODE;
#define MB_COMMAND_READ_COIL 0x01
#define MB_COMMAND_READ_INPUT_STAT 0X02
#define MB_COMMAND_READ_HOLDING_REG 0x03
#define MB_COMMAND_READ_INPUT_REG 0x04
#define MB_COMMAND_SET_SINGLE_COIL 0x05
#define MB_COMMAND_SET_SINGLE_REG 0x06
#define MB_COMMAND_SET_MANY_COIL 0x0F
#define MB_COMMAND_SET_MANY_REG 0x10

#pragma endregion

#pragma region PAYLOADS

typedef struct{
    uint8_t slaveAddr;
    MB_COMMAND_CODE functionCode;
    uint16_t startAdress;
    uint16_t nRegisters;
    uint16_t crc16;
} MB_payload_read;

typedef MB_payload_read MB_payload_read_holding;
typedef MB_payload_read MB_payload_read_input_reg;
typedef MB_payload_read MB_payload_read_coil;
typedef MB_payload_read MB_payload_read_input_stat;

typedef struct{
    uint8_t slaveAddr;
    MB_COMMAND_CODE functionCode;
    uint8_t responseLen;
    uint8_t* data;
    uint16_t crc16;  
} MB_response;

#pragma endregion

#pragma region FUNCTIONS
uint16_t swapEndian16(uint16_t in);
void MB_initPayload_readHoldingReg( MB_payload_read_holding*    payload,uint8_t slave,uint16_t start,uint16_t numRegisters);
void MB_initPayload_readInputReg(   MB_payload_read_input_reg*  payload,uint8_t slave,uint16_t start,uint16_t numRegisters);
void MB_initPayload_readCoil(       MB_payload_read_coil*       payload,uint8_t slave,uint16_t start,uint16_t numRegisters);
void MB_initPayload_readInputStat(  MB_payload_read_input_stat* payload,uint8_t slave,uint16_t start,uint16_t numRegisters);

char* MB_strerror(int error);
int MB_initResponse(MB_response* response, uint8_t* data, int data_len);
void MB_getRegisters(MB_response* response ,uint16_t** registers, int* registerCount);
int MB_init_payload_from_str(MB_payload_read* payload,char* string);


#pragma endregion

#endif