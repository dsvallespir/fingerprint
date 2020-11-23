#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#define VFY_PWD                     0x13
#define SET_PWD                     0x12
#define SET_ADDR                    0x15
#define SYS_PARAM                   0x0E
#define PORT_CTRL                   0x17
#define GET_SYS_PARAM               0x0F
#define READ_TMPL_NUM               0x1D
#define GEN_IMG                     0x01
#define UP_IMG                      0x0A
#define DOWN_IMG                    0x0B
#define GEN_TEMP_FROM_IMG           0x02
#define GEN_CHAR_FILE_FROM_IMG      0x05
#define UP_CHAR                     0x08
#define DOWN_CHAR                   0x09
#define STORE_TMPL                  0x06
#define READ_TMPL_FROM_FLASH        0x07
#define DEL_TMPL_FROM_FLASH         0x0C
#define EMPTY_LIBRARY               0x0D
#define MATCH_TEMPL                 0x03
#define SEARCH_TEMPL                0x04
#define GEN_RANDOM                  0x14
#define WRITE_NOTEPAD               0x18
#define READ_NOTEPAD                0x19

#define PID_CMD_PACKET              0x01
#define PID_DATA_PACKET             0x02
#define PID_ACK_PACKET              0x07
#define PID_END_PACKET              0x08

#define CONF_CODE_CMD_EXEC_COMPL        0x00
#define CONF_CODE_ERR_RECV_DATA_PCKG    0x01
#define CONF_CODE_NO_FINGER             0x02
#define CONF_CODE_FAIL_TO_ENROLL        0x03




typedef enum{
    STATE_IDLE,
    STATE_PACKET_ID,
    STATE_CMD_PACKET,
    STATE_DATA_PACKET,
    STATE_ACK_PACKET,
    STATE_END_PACKET,
    STATE_CONFIRMATION_CODE,
    STATE_PROCESS_DATA,
    STATE_ERROR
} stateType_t;

struct SysParamStructure{
    unsigned char statReg[2];
    unsigned char sysIdCode[2];
    unsigned char fingerLibSize[2];
    unsigned char securityLevel[2];
    unsigned char deviceAddress[4];
    unsigned char dataSize[2];
    unsigned char baudSettings[2];
};


#endif // FINGERPRINT_H
