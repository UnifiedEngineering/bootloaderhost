#ifndef CYADDON_H
#define CYADDON_H

#include <stdint.h>
#include <cybtldr_api.h>

typedef struct {
    uint16_t appId;
    uint16_t appVer;
    uint16_t bootVer;
    uint16_t fileAppId;
    uint16_t fileAppVer;
    uint32_t appCustId;
    uint32_t fileAppCustId;
} cymeta_t;

// Get Metadata command not included in current version of cybootloaderutils
#define CMD_GET_METADATA 0x3C

// Offsets for the info we need in the metadata section
// BTLDR is the bootloader, BTLDB is the application (Bootloadable)
// Values are claimed to be big-endian for PSoC3 and little-endian for PSoC5?
#define BTLDR_APP_VERSION_OFFSET (18) // 2 bytes
#define BTLDB_APP_ID_OFFSET (20) // 2 bytes
#define BTLDB_APP_VERSION_OFFSET (22) // 2 bytes
#define BTLDB_APP_CUST_ID_OFFSET (24) // 4 bytes

int CyBtldr_GetMetadata(CyBtldr_CommunicationsData* cyComms, const char* filename, uint8_t bootloadableNum, cymeta_t* meta);
int CyBtldr_CreateGetMetadataCmd(uint8_t* cmdBuf, uint16_t* cmdSize, uint16_t* resSize, uint8_t bootloadableNum);
int CyBtldr_ParseGetMetadataCmdResult(uint8_t* cmdBuf, uint16_t cmdSize, cymeta_t* meta);

#endif // CYADDON_H

