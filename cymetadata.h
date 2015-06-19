#ifndef CYADDON_H
#define CYADDON_H

#include <stdint.h>

// Get Metadata command not included in current version of cybootloaderutils
#define CMD_GET_METADATA 0x3C

// Offsets for the info we need in the metadata section
// BTLDR is the bootloader, BTLDB is the application (Bootloadable)
// Values are claimed to be big-endian for PSoC3 and little-endian for PSoC5?
#define BTLDR_APP_VERSION_OFFSET (18) // 2 bytes
#define BTLDB_APP_ID_OFFSET (20) // 2 bytes
#define BTLDB_APP_VERSION_OFFSET (22) // 2 bytes
#define BTLDB_APP_CUST_ID_OFFSET (24) // 4 bytes

int CyBtldr_GetMetadata(uint8_t bootloadableNum, uint32_t* appCustId, uint16_t* appId, uint16_t* appVer, uint16_t* bootVer);
int CyBtldr_CreateGetMetadataCmd(uint8_t* cmdBuf, uint16_t* cmdSize, uint16_t* resSize, uint8_t bootloadableNum);
int CyBtldr_ParseGetMetadataCmdResult(uint8_t* cmdBuf, uint16_t cmdSize, uint32_t* appCustId, uint16_t* appId, uint16_t* appVer, uint16_t* bootVer);

#endif // CYADDON_H

